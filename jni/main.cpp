/* actual mod that doesnt boot
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

// Scans the running process maps to find the game library's base address
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

// Safely overwrites game instructions in memory
void patchBytes(uintptr_t address, const void* bytes, size_t size) {
    uintptr_t page_size = sysconf(_SC_PAGE_SIZE);
    uintptr_t page_start = (address & ~(page_size - 1));
    
    // Temporarily unlock memory page protection to make it writable
    mprotect((void*)page_start, page_size, PROT_READ | PROT_WRITE | PROT_EXEC);
    
    // Overwrite the original game bytes with your modded bytes
    memcpy((void*)address, bytes, size);
    
    // Lock the memory page back down to keep the game stable
    mprotect((void*)page_start, page_size, PROT_READ | PROT_EXEC);
}

void* hack_thread(void*) {
    // Wait for the game library to load into memory
    while (libBase == 0) {
        libBase = get_library_base("libmortargame.so");
        if (libBase == 0) {
            sleep(1);
        }
    }

    LOGI("Found libmortargame.so base address at: %p", (void*)libBase);

    // Target specific fruit generation offset
    uintptr_t targetAddress = libBase + 0x01daa2b0;

    // Hex bytes for modification
    // This tells the CPU: "Load the number 5 into the return register, then return"
    unsigned char bananaPatch[] = { 0x05, 0x00, 0x80, 0xD2, 0xC0, 0x5F, 0x03, 0xC0 }; 

    // Apply the patch directly into the game's live memory
    patchBytes(targetAddress, bananaPatch, sizeof(bananaPatch));
    LOGI("Banana modification successfully written to memory!");

    return NULL;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    pthread_t pt;
    pthread_create(&pt, NULL, hack_thread, NULL);
    return JNI_VERSION_1_6;
}
*/
///////////////////////////////////////////////////////////////////////
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

// Scans the running process maps to find the game library's base address
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

void* hack_thread(void*) {
    // Wait for the game library to load into memory
    while (libBase == 0) {
        libBase = get_library_base("libmortargame.so");
        if (libBase == 0) {
            sleep(1);
        }
    }

    LOGI("Found libmortargame.so base address at: %p", (void*)libBase);
    LOGI("Running safe mode (no patches applied yet).");

    return NULL;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    pthread_t pt;
    pthread_create(&pt, NULL, hack_thread, NULL);
    return JNI_VERSION_1_6;
}
