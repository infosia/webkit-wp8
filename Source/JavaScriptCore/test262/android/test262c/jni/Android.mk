# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := Test262
LOCAL_SRC_FILES := Test262_JNI.c \
Test262HelperAndroid.c \
Logger_wrap.c \
../../../common/source/test262.c \
../../../common/source/CommonUtils.c \
../../../common/source/LogWrapper.c \
../../../common/source/Logger/clogger/Logger.c \
../../../common/source/Logger/clogger/LoggerPrimitives.c \
../../../common/source/Logger/timestamp/TimeStamp.c \
../../../common/source/Logger/CustomAdapterExamples/AndroidLog/LoggerAdapter__android_log.c \
../../../common/source/SimpleThread/SimpleMutex.c \
../../../common/source/SimpleThread/SimpleThreadPosix.c \
../../../common/source/SocketServer.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../API/ 
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../WTF/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../common/source/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../common/source/Logger/clogger
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../common/source/Logger/timestamp
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../common/source/Logger/CustomAdapterExamples/AndroidLog
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../common/source/SimpleThread

LOCAL_CFLAGS := -std=c99
# http://blog.algolia.com/android-ndk-how-to-reduce-libs-size/
LOCAL_CFLAGS += -DLOGGER_ENABLE_LOCKING
LOCAL_CFLAGS += -DNDEBUG
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -ffunction-sections -fdata-sections 
LOCAL_LDFLAGS += -Wl,--gc-sections

#LOCAL_SHARED_LIBRARIES := ALmixer_shared openal_shared
#LOCAL_SHARED_LIBRARIES := openal_shared
LOCAL_SHARED_LIBRARIES := JavaScriptCore_shared
LOCAL_LDLIBS    := -llog -landroid



include $(BUILD_SHARED_LIBRARY)

# Remember: The NDK_MODULE_PATH environmental variable must contain the modules directories in the search path.
# android build system will look for folder `ALmixer` and `openal`
# in all import paths:
#$(call import-module,ALmixer)
#$(call import-module,openal-soft)
$(call import-module,JavaScriptCore)

