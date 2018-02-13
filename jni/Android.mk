LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS += -fPIE -pie
LOCAL_MODULE    := target
LOCAL_SRC_FILES := target.c
cmd-strip :=  

include $(BUILD_EXECUTABLE)

###########################################################

include $(CLEAR_VARS)
LOCAL_MODULE    := hook
#LOCAL_SRC_FILES := hook.c.arm
LOCAL_SRC_FILES := hook.c

include $(BUILD_STATIC_LIBRARY)

###########################################################

include $(CLEAR_VARS)
LOCAL_MODULE    := relocate
LOCAL_SRC_FILES := relocate.c

include $(BUILD_STATIC_LIBRARY)

###########################################################


include $(CLEAR_VARS)
LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS += -fPIE -pie

LOCAL_MODULE    := stalker
LOCAL_SRC_FILES := stalker.c
LOCAL_LDLIBS := -llog 
LOCAL_STATIC_LIBRARIES := hook

include $(BUILD_EXECUTABLE)

###########################################################

include $(CLEAR_VARS)
LOCAL_DISABLE_FATAL_LINKER_WARNINGS := true
LOCAL_MODULE    := inject
LOCAL_SRC_FILES := inject.c
LOCAL_LDLIBS := -llog 
LOCAL_STATIC_LIBRARIES := hook relocate

include $(BUILD_SHARED_LIBRARY)

###########################################################

include $(CLEAR_VARS)
LOCAL_DISABLE_FATAL_LINKER_WARNINGS := true
LOCAL_MODULE    := realinject
LOCAL_SRC_FILES := realinject.c
LOCAL_LDLIBS := -llog 
LOCAL_STATIC_LIBRARIES := hook relocate

include $(BUILD_SHARED_LIBRARY)
