LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

FREETYPE_SRC_PATH := ../../

LOCAL_MODULE := freetype2-static

LOCAL_CFLAGS := -DANDROID_NDK \
		-DFT2_BUILD_LIBRARY=1

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include_all \
		$(FREETYPE_SRC_PATH)include \
		$(FREETYPE_SRC_PATH)src

LOCAL_SRC_FILES := \
	$(FREETYPE_SRC_PATH)src/autofit/autofit.c \
	$(FREETYPE_SRC_PATH)src/base/basepic.c \
	$(FREETYPE_SRC_PATH)src/base/ftapi.c \
	$(FREETYPE_SRC_PATH)src/base/ftbase.c \
	$(FREETYPE_SRC_PATH)src/base/ftbbox.c \
	$(FREETYPE_SRC_PATH)src/base/ftbitmap.c \
	$(FREETYPE_SRC_PATH)src/base/ftdbgmem.c \
	$(FREETYPE_SRC_PATH)src/base/ftdebug.c \
	$(FREETYPE_SRC_PATH)src/base/ftglyph.c \
	$(FREETYPE_SRC_PATH)src/base/ftinit.c \
	$(FREETYPE_SRC_PATH)src/base/ftpic.c \
	$(FREETYPE_SRC_PATH)src/base/ftstroke.c \
	$(FREETYPE_SRC_PATH)src/base/ftsynth.c \
	$(FREETYPE_SRC_PATH)src/base/ftsystem.c \
	$(FREETYPE_SRC_PATH)src/cff/cff.c \
	$(FREETYPE_SRC_PATH)src/pshinter/pshinter.c \
	$(FREETYPE_SRC_PATH)src/psnames/psnames.c \
	$(FREETYPE_SRC_PATH)src/raster/raster.c \
	$(FREETYPE_SRC_PATH)src/sfnt/sfnt.c \
	$(FREETYPE_SRC_PATH)src/smooth/smooth.c \
	$(FREETYPE_SRC_PATH)src/truetype/truetype.c

LOCAL_LDLIBS := -ldl -llog

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := placeholder-jni
LOCAL_STATIC_LIBRARIES := freetype2-static
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_SRC_FILES := dummy.c

include $(BUILD_SHARED_LIBRARY)
