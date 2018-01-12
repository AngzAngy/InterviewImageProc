LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#OPENCV_CAMERA_MODULES:=off
#OPENCV_INSTALL_MODULES:=off
#OPENCV_LIB_TYPE:=SHARED
include ../../sdk/native/jni/OpenCV.mk

GPUSRC := gpu/src

LOCAL_SRC_FILES  := FaceDetect_jni.cpp jRect.cpp \
                   $(GPUSRC)/EglGlContext.cpp\
                   $(GPUSRC)/GLProgram.cpp\
                   $(GPUSRC)/GLTexture2d.cpp\
                   $(GPUSRC)/FilterMatrix.cpp\
                   $(GPUSRC)/Framebuffer.cpp\
                   $(GPUSRC)/GLRender.cpp \
                   MakeupRender.cpp\
                   Makeup_jni.cpp

LOCAL_C_INCLUDES += $(LOCAL_PATH)  $(LOCAL_PATH)/gpu/include

LOCAL_LDLIBS     += -llog -ldl -ljnigraphics -lGLESv2 -lEGL

LOCAL_MODULE     := face_detect_jni

include $(BUILD_SHARED_LIBRARY)
