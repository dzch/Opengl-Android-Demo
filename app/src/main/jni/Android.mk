# Tell Android where to locate source files
# "my-dir" is a macro function which will return the path of the current directory(where Android.mk resides)
LOCAL_PATH := $(call my-dir)

# Clear contents of the LOCAL_* variables
include $(CLEAR_VARS)

# All the source files to include in this module
LOCAL_SRC_FILES := nativeLibrary.c \
                   jniExports.c 

# The name of the module
LOCAL_MODULE := libdemo

# Compilation flags
LOCAL_CFLAGS := -g -Wall -Wextra

# Static libraries to link with
LOCAL_LDLIBS := -lGLESv2 -lEGL -llog

# Build the module
include $(BUILD_SHARED_LIBRARY)
