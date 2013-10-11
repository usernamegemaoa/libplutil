LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libplutil
LOCAL_SRC_FILES := plconfig.c
LOCAL_CFLAGS += -Wall -O2
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../external/eglib
include $(BUILD_STATIC_LIBRARY)
