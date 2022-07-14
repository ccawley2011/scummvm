/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#define FORBIDDEN_SYMBOL_ALLOW_ALL
#include "common/scummsys.h"

#if defined(DYNAMIC_MODULES) && defined(RISCOS)

#include "backends/plugins/riscos/riscos-provider.h"
#include "backends/plugins/elf/arm-loader.h"

#include "common/debug.h"

#include <kernel.h>
#include <swis.h>

// HACK: This is needed so that standard library functions that are only
// used in plugins can be found in the main executable.
void pluginHack() {
	volatile float f = 0.0f;
	volatile double d = 0.0;
	volatile int i = 0;

	byte *b = new (std::nothrow) byte[100];

	f = tanhf(f);
	f = logf(f);
	f = log10f(f);
	f = lroundf(f);
	f = expf(f);
	f = frexpf(f, NULL);
	f = ldexpf(f, 1);
	f = fmaxf(f, f);
	f = fminf(f, f);
	f = truncf(f);

	d = nearbyint(d);

	i = strcoll("dummyA", "dummyB");

	rename("dummyA", "dummyB");

	delete[] b;
}

class RiscOSDLObject : public ARMDLObject {
protected:
	void flushDataCache(void *ptr, uint32 len) const override {
		_kernel_swi_regs regs;

		regs.r[0] = 1;
		regs.r[1] = (int)ptr;
		regs.r[2] = (int)ptr + len;

		_kernel_swi(OS_SynchroniseCodeAreas, &regs, &regs);
	}
};

/**
 * On 26-bit RISC OS, plugins need to be allocated in the first 64 MB
 * of RAM so that it can be executed. This may not be the case when using
 * the default allocators, which use dynamic areas for large allocations.
 *
 * TODO: Use the application slot instead?
 * TODO: Make more of an effort to free the memory in the event of a crash.
 */
class RiscOSDLObject_RMA : public RiscOSDLObject {
protected:
	void *allocateMemory(uint32 align, uint32 size) override {
		_kernel_swi_regs regs;

		if (align < sizeof(uintptr))
			align = sizeof(uintptr);

		regs.r[0] = 6;
		regs.r[3] = size + align + sizeof(uintptr);

		if (_kernel_swi(OS_Module, &regs, &regs) != NULL)
			return NULL;

		uintptr ptr = (regs.r[2] + align - 1) & ~(align - 1);
		*((uintptr *)ptr - 1) = regs.r[2];

		return (void *)ptr;

	}

	void deallocateMemory(void *ptr, uint32 size) override {
		_kernel_swi_regs regs;

		regs.r[0] = 7;
		regs.r[2] = *((uintptr *)ptr - 1);

		_kernel_swi(OS_Module, &regs, &regs);
	}
};

RiscOSPluginProvider::RiscOSPluginProvider() : _is32bit(false) {
}

Plugin *RiscOSPluginProvider::createPlugin(const Common::FSNode &node) const {
	if (_is32bit) {
		return new TemplatedELFPlugin<RiscOSDLObject>(node.getPath());
	} else {
		return new TemplatedELFPlugin<RiscOSDLObject_RMA>(node.getPath());
	}
}

#endif // defined(DYNAMIC_MODULES) && defined(RISCOS)
