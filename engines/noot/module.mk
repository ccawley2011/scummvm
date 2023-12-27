MODULE := engines/noot

MODULE_OBJS = \
	noot.o \
	animation.o \
	book.o \
	console.o \
	decompress.o \
	dialogs.o \
	metaengine.o \
	widgets.o

# This module can be built as a plugin
ifeq ($(ENABLE_NOOT), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
