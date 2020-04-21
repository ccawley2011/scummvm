/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include <nds.h>
#include <fat.h>

#include "backends/platform/ds/arm9/source/osystem_ds.h"
#include "backends/platform/ds/arm9/source/dsmain.h"
#include "backends/plugins/ds/ds-provider.h"
#include "base/main.h"
#include "base/version.h"

namespace DS {

///////////////////
// Fast Ram
///////////////////

#define FAST_RAM_SIZE (22500)
#define ITCM_DATA	__attribute__((section(".itcm")))

u8 *fastRamPointer;
u8 fastRamData[FAST_RAM_SIZE] ITCM_DATA;

void *fastRamAlloc(int size) {
	void *result = (void *) fastRamPointer;
	fastRamPointer += size;
	if(fastRamPointer > fastRamData + FAST_RAM_SIZE) {
		iprintf("FastRam (ITCM) allocation failed!\n");
		return malloc(size);
	}
	return result;
}

void fastRamReset() {
	fastRamPointer = &fastRamData[0];
}

}

int main(int argc, char *argv[]) {
	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);
	vramSetBankH(VRAM_H_SUB_BG);
	consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false, true);

	iprintf("-------------------------------\n");
	iprintf("ScummVM DS\n");
	iprintf("Version %s ", gScummVMVersion);
	iprintf("-------------------------------\n");
	iprintf("L/R + D-pad/pen:    Scroll view\n");
	iprintf("D-pad left:   Left mouse button\n");
	iprintf("D-pad right: Right mouse button\n");
	iprintf("D-pad up:           Hover mouse\n");
	iprintf("B button:        Skip cutscenes\n");
	iprintf("Select:         DS Options menu\n");
	iprintf("Start:   Game menu (some games)\n");
	iprintf("Y (in game):     Toggle console\n");
	iprintf("X:              Toggle keyboard\n");
	iprintf("A:                 Swap screens\n");
	iprintf("L+R (on start):      Clear SRAM\n");
	iprintf("\n");

	if (!fatInitDefault()) {
		iprintf("fatInitDefault failed\n");
	}

	g_system = new OSystem_DS();
	assert(g_system);

#ifdef DYNAMIC_MODULES
	PluginManager::instance().addPluginProvider(new DSPluginProvider());
#endif

	// Invoke the actual ScummVM main entry point:
	int res = scummvm_main(argc, argv);

	// Free OSystem
	g_system->destroy();

	return res;
}
