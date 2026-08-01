#include <jni.h>
#include <string>
#include <cstdlib>
#include "Includes/KittyMemory/MemoryPatch.h" 
#include "Includes/Logger.h"

// Memory offset we found in Ghidra for Fruit::RandomFruit
uintptr_t libBase = 0; // Automatically resolved by Zygisk/Mod template
#define OFFSET_RANDOM_FRUIT 0x01daa2b0

// Original function pointer
unsigned long (*Old_RandomFruit)(void* instance, bool param_1);

// Hooked function to modify fruit selection
unsigned long Hooked_RandomFruit(void* instance, bool param_1) {
    // Call the game's original random fruit logic
    unsigned long fruitID = Old_RandomFruit(instance, param_1);

    // Define your Frenzy Banana ID (typically 5 or whichever ID it is in your game)
    int frenzyBananaID = 5; 

    // --- 2x SPAWN RATE LOGIC ---
    // If it's not a frenzy banana, give it a balanced chance to turn into one
    if (fruitID != frenzyBananaID) {
        if (rand() % 100 < 15) { // Roughly increases spawn frequency
            fruitID = frenzyBananaID;
        }
    }

    // --- 50% NEW BANANA VARIANT LOGIC ---
    // When a frenzy banana spawns, apply a 50% chance modifier 
    if (fruitID == frenzyBananaID) {
        if (rand() % 2 == 0) {
            // This is where your custom bottom-spawning logic variant triggers
            // You can route or modify behavior here if needed
        }
    }

    return fruitID;
}

// Setup the hook when the library loads
void* hack_thread(void*) {
    // Wait for the game library to load into memory
    while (libBase == 0) {
        libBase = KittyMemory::getLibraryBase("libmortargame.so");
        sleep(1);
    }

    LOGI("libmortargame.so base address found: %p", (void*)libBase);

    // Apply the hook to Fruit::RandomFruit
    MSHookFunction((void*)(libBase + OFFSET_RANDOM_FRUIT), (void*)Hooked_RandomFruit, (void**)&Old_RandomFruit);
    
    LOGI("Successfully hooked Fruit::RandomFruit!");
    return NULL;
}

// Android entry point
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    pthread_t pt;
    pthread_create(&pt, NULL, hack_thread, NULL);
    return JNI_VERSION_1_6;
}
