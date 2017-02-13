# A simple test for the minimal standard C++ library
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
#LOCAL_CFLAGS += -fPIE -mfloat-abi=softfp -mfpu=neon
LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS += -fPIE -pie
#LOCAL_ARM_MODE := arm
LOCAL_ARM_NEON := true
#TARGET_ARCH_ABI := armeabi-v7a
LOCAL_MODULE := test-neon
LOCAL_SRC_FILES := test-neon.cpp convert_asm.s
#LOCAL_SRC_FILES := test-neon.cpp
#LOCAL_FILTER_ASM := cp
include $(BUILD_EXECUTABLE)
