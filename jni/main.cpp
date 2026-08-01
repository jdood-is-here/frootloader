#include <jni.h>
#include <string>
#include <cstdlib>
#include "Includes/KittyMemory/MemoryPatch.h" 
#include "Includes/Logger.h"

// Memory offset found in Ghidra for Fruit::RandomFruit
uintptr_t libBase = 0; 
#define OFFSET_RANDOM_FRUIT 0x01daa2b0

// Global flag to signal our bottom-spawning variant state
bool g_BottomSpawnFrenzyActive = false;

// Original function pointer
unsigned long (*Old_RandomFruit)(void* instance, bool param_1);

// Hooked function to modify fruit selection and split frenzy variants
unsigned long Hooked_RandomFruit(void* instance, bool param_1) {
    // Call the game's original random fruit logic
    unsigned long fruitID = Old_RandomFruit(instance, param_1);

    int frenzyBananaID = 5; // Adjust to match your game's internal frenzy ID if necessary

    // Step 1: Boost general Frenzy Banana frequency (approx. 15% chance boost)
    if (fruitID != frenzyBananaID) {
        if (rand() % 100 < 15) {
            fruitID = frenzyBananaID;
        }
    }

    // Step 2: When a Frenzy Banana spawns, apply a 50% split for the bottom-spawning variant
    if (fruitID == frenzyBananaID) {
        if (rand() % 2 == 0) {
            g_BottomSpawnFrenzyActive = true;
            LOGI("Custom Variant Triggered: Bottom-Spawning Frenzy Banana!");
        } else {
            g_BottomSpawnFrenzyActive = false;
        }
    } else {
        g_BottomSpawnFrenzyActive = false;
    }

    return fruitID;
}

// Setup the hook when the library loads
void* hack_thread(void*) {
    while (libBase == 0) {
        libBase = KittyMemory::getLibraryBase("libmortargame.so");
        sleep(1);
    }

    LOGI("libmortargame.so base address found: %p", (void*)libBase);

    // Apply the hook to Fruit::RandomFruit
    MSHookFunction((void*)(libBase + OFFSET_RANDOM_FRUIT), (void*)Hooked_RandomFruit, (void**)&Old_RandomFruit);
    
    LOGI("Successfully hooked Fruit::RandomFruit with 50/50 split variant logic!");
    return NULL;
}

// Android entry point
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    pthread_t pt;
    pthread_create(&pt, NULL, hack_thread, NULL);
    return JNI_VERSION_1_6;
}
