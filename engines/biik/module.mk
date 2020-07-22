MODULE := engines/biik

MODULE_OBJS = \
	archive.o \
	biik.o \
	console.o \
	decompress.o \
	detection.o \
	script.o \
	sound.o \
	gui/cursors.o \
	gui/element.o \
	gui/gui.o \
	gui/mac_gui.o \
	gui/riscos_gui.o \
	gui/riscos_template.o \
	gui/windows_gui.o

# This module can be built as a plugin
ifeq ($(ENABLE_BIIK), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
