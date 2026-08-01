#include <jni.h>
#include <string>
#include <cstdlib>
#include "KittyMemory/MemoryPatch.h" 
#include "Includes/Logger.h"

uintptr_t libBase = 0; 
#define OFFSET_RANDOM_FRUIT 0x01daa2b0

bool g_BottomSpawnFrenzyActive = false;
unsigned long (*Old_RandomFruit)(void* instance, bool param_1);

unsigned long Hooked_RandomFruit(void* instance, bool param_1) {
    unsigned long fruitID = Old_RandomFruit(instance, param_1);

    // Replace with your verified Frenzy Banana ID from logs
    int frenzyBananaID = 5; 

    // Boost general Frenzy Banana frequency (approx. 15% chance boost)
    if (fruitID != frenzyBananaID) {
        if (rand() % 100 < 15) {
            fruitID = frenzyBananaID;
        }
    }

    // 50/50 split variant logic for bottom-spawning frenzy
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

void* hack_thread(void*) {
    while (libBase == 0) {
        libBase = KittyMemory::getLibraryBase("libmortargame.so");
        sleep(1);
    }

    LOGI("libmortargame.so base address found: %p", (void*)libBase);

    MSHookFunction((void*)(libBase + OFFSET_RANDOM_FRUIT), (void*)Hooked_RandomFruit, (void**)&Old_RandomFruit);
    
    LOGI("Successfully hooked Fruit::RandomFruit!");
    return NULL;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    pthread_t pt;
    pthread_create(&pt, NULL, hack_thread, NULL);
    return JNI_VERSION_1_6;
}
