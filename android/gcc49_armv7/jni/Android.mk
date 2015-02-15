LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS) 

TARGET_ARCH_ABI := armeabi-v7a
LOCAL_ARM_NEON := true
LOCAL_MODULE := taltospp

LOCAL_CPPFLAGS += -std=c++11
LOCAL_CPP_FEATURES += exceptions
LOCAL_C_INCLUDES += ${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/4.9/include
LOCAL_C_INCLUDES += ${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../tests
LOCAL_C_INCLUDES += $(LOCAL_PATH)

ROOT_PATH := ../../..
include $(LOCAL_PATH)/../../../android/sources

include $(BUILD_EXECUTABLE)
