#include <jni.h>
#include <unistd.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <android/log.h>

#define LOG_TAG "FruitMod"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

uintptr_t libBase = 0;

uintptr_t get_library_base(const char* library_name) {
    uintptr_t address = 0;
    char filename[32];
    snprintf(filename, sizeof(filename), "/proc/%d/maps", getpid());
    FILE* fp = fopen(filename, "r");
    if (fp) {
        char line[512];
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, library_name)) {
                uintptr_t base;
                if (sscanf(line, "%lx-%*s", &base) == 1) {
                    address = base;
                    break;
                }
            }
        }
        fclose(fp);
    }
    return address;
}

void patchBytes(uintptr_t address, const void* bytes, size_t size) {
    uintptr_t page_size = sysconf(_SC_PAGE_SIZE);
    uintptr_t page_start = (address & ~(page_size - 1));
    
    mprotect((void*)page_start, page_size, PROT_READ | PROT_WRITE | PROT_EXEC);
    memcpy((void*)address, bytes, size);
    mprotect((void*)page_start, page_size, PROT_READ | PROT_EXEC);
}

void* hack_thread(void*) {
    while (libBase == 0) {
        libBase = get_library_base("libmortargame.so");
        if (libBase == 0) {
            sleep(1);
        }
    }

    LOGI("Found libmortargame.so base address at: %p", (void*)libBase);

    // Target the SpawnMultiplier variable address
    uintptr_t targetAddress = libBase + 0x01daa2b0;

    // Hex bytes for the float value 2.0 (Doubles the spawn rate)
    unsigned char multiplierPatch[] = { 0x00, 0x00, 0x00, 0x40 }; 

    patchBytes(targetAddress, multiplierPatch, sizeof(multiplierPatch));
    LOGI("Arcade banana spawn rate successfully doubled!");

    return NULL;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    pthread_t pt;
    pthread_create(&pt, NULL, hack_thread, NULL);
    return JNI_VERSION_1_6;
}
