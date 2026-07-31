LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := ModMenu
LOCAL_SRC_FILES := main.cpp
LOCAL_LDLIBS := -llog -landroid
include $(BUILD_SHARED_LIBRARY)
