#include <jni.h>
#include <string>
#include <unistd.h>
#include "KittyMemory/MemoryPatch.h"
#include "Logger.h"

uintptr_t libBase = 0;

void* hack_thread(void*) {
    // Wait for libmortargame.so to load into memory
    while (libBase == 0) {
        libBase = KittyMemory::getLibraryBase("libmortargame.so");
        if (libBase == 0) {
            sleep(1);
        }
    }

    LOGI("libmortargame.so base address found at: %p", (void*)libBase);

    // Add your memory patches here using KittyMemory if needed, for example:
    // MemoryPatch frenzyPatch = MemoryPatch::createWithHex(libBase + 0x1DAA2B0, "00 00 A0 E3");
    // frenzyPatch.Modify();

    LOGI("Mod template initialized successfully!");
    return NULL;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    pthread_t pt;
    pthread_create(&pt, NULL, hack_thread, NULL);
    return JNI_VERSION_1_6;
}
