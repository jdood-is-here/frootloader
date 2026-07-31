#include <jni.h>
#include <android/log.h>

#define TAG "FrootLoader"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

extern "C" {
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGI("FrootLoader successfully injected into memory!");
    return JNI_VERSION_1_6;
}
}
