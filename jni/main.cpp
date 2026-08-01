#include <jni.h>
#include <unistd.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <android/log.h>

#define LOG_TAG "FruitMod"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Function to find the base address of the game library from /proc/self/maps
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
    uintptr_t libBase = 0;
    
    // Wait until the game library is fully loaded into memory
    while (libBase == 0) {
        libBase = get_library_base("libmortargame.so");
        if (libBase == 0) {
            sleep(1);
        }
    }

    LOGI("Successfully found libmortargame.so base address at: %p", (void*)libBase);

    // Ready for memory patching or custom logic here!

    return NULL;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    pthread_t pt;
    pthread_create(&pt, NULL, hack_thread, NULL);
    return JNI_VERSION_1_6;
}
