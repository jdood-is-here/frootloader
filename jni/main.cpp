#include <jni.h>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <dlfcn.h>
#include <substrate.h>

uintptr_t libBase = 0; 
#define OFFSET_RANDOM_FRUIT 0x01daa2b0

bool g_BottomSpawnFrenzyActive = false;
unsigned long (*Old_RandomFruit)(void* instance, bool param_1);

unsigned long Hooked_RandomFruit(void* instance, bool param_1) {
    unsigned long fruitID = Old_RandomFruit(instance, param_1);

    int frenzyBananaID = 5; 

    if (fruitID != frenzyBananaID) {
        if (rand() % 100 < 15) {
            fruitID = frenzyBananaID;
        }
    }

    if (fruitID == frenzyBananaID) {
        if (rand() % 2 == 0) {
            g_BottomSpawnFrenzyActive = true;
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
        libBase = reinterpret_cast<uintptr_t>(dlopen("libmortargame.so", RTLD_NOLOAD));
        if (libBase == 0) {
            sleep(1);
        }
    }

    MSHookFunction(reinterpret_cast<void*>(libBase + OFFSET_RANDOM_FRUIT), 
                   reinterpret_cast<void*>(Hooked_RandomFruit), 
                   reinterpret_cast<void**>(&Old_RandomFruit));
    
    return NULL;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    pthread_t pt;
    pthread_create(&pt, NULL, hack_thread, NULL);
    return JNI_VERSION_1_6;
}
