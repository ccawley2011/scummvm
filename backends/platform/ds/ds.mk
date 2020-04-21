# Set location of ndsdir so that we can easily refer to files in it
ndsdir = backends/platform/ds

LOGO = logo.bmp

# Compiler options for files which should be optimised for speed
OPT_SPEED := -O3 -marm

# Compiler options for files which should be optimised for space
OPT_SIZE := -Os -mthumb

# By default optimize for size
CXXFLAGS += $(OPT_SIZE)

# Files listed below will be optimisied for speed, otherwise they will be optimised for space.
# TODO: Several of these files probably should not be optimized for speed, but for now
# we replicate the *precise* list from the old DS makefile, to ensure full compatibility.
# Eventually, we should tune this list.
$(ndsdir)/arm9/source/blitters.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
$(ndsdir)/arm9/source/osystem_ds.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
base/main.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
sound/rate.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
sound/softsynth/opl/mame.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/agi/sprite.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/agos/gfx.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/agos/script.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/cine/gfx.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/cruise/actor.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/cruise/script.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/draci/script.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/draci/sprite.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/gob/script.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/groovie/script.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/kyra/script.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/m4/actor.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/m4/script.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/m4/sprite.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/made/script.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/saga/actor_path.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/saga/actor_walk.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/saga/actor.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/saga/gfx.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/saga/image.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/saga/isomap.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/saga/script.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/saga/sprite.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/sci/engine/script.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/scumm/actor.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/scumm/gfx.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/scumm/script.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/sword2/sprite.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
engines/teenagent/actor.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)
# TODO: Fingolfin says: optimizing kyra/staticres.o for size would
# save about 30k, so maybe consider that?
#engines/kyra/staticres.o: CXXFLAGS:=$(CXXFLAGS) $(OPT_SPEED)



#############################################################################
#
# ARM9 rules.
#
#############################################################################

all: scummvm.nds

clean: dsclean

dsclean:
	$(RM) $(addprefix $(ndsdir)/, $(ARM7_MODULE_OBJS)) scummvm.nds

.PHONY: dsclean

# TODO: Add a 'dsdist' target ?

%.nds: %.elf
	ndstool -c $@ -9 $< -b $(srcdir)/$(ndsdir)/$(LOGO) "$(@F);ScummVM $(VERSION);DS Port"





# Command to build libmad is:
# ./configure --host=arm-elf --enable-speed --enable-sso -enable-fpm=arm CFLAGS='-specs=ds_arm9.specs -mthumb-interwork'
#
# I actually had to use
# ./configure --host=arm-elf --enable-speed --enable-sso -enable-fpm=arm CFLAGS='-specs=ds_arm9.specs -mthumb-interwork' LDFLAGS='C:/Progra~1/devkitpro/libnds/lib/libnds9.a' --disable-shared --disable-debugging
#
# Fingolfin used
# CXX=arm-eabi-g++ CC=arm-eabi-gcc ./configure --host=arm-elf --enable-speed --enable-sso -enable-fpm=arm CFLAGS='-specs=ds_arm9.specs -mthumb-interwork' --disable-shared --disable-debugging LDFLAGS=$DEVKITPRO/libnds/lib/libnds9.a
