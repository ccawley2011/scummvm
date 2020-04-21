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


#ifndef _OSYSTEM_DS_H_
#define _OSYSTEM_DS_H_

// Allow use of stuff in <nds.h>
#define FORBIDDEN_SYMBOL_EXCEPTION_printf
#define FORBIDDEN_SYMBOL_EXCEPTION_unistd_h

#include "backends/modular-backend.h"

class OSystem_DS : public ModularBackend, Common::EventSource {
public:
	OSystem_DS();
	virtual ~OSystem_DS();

public:
	virtual Common::EventSource *getDefaultEventSource() { return this; }
	virtual bool pollEvent(Common::Event &event) override;
	virtual Common::HardwareInputSet *getHardwareInputSet() override;

	virtual uint32 getMillis(bool skipRecord = false) override;
	virtual void delayMillis(uint msecs) override;
	virtual void getTimeAndDate(TimeDate &t) const override;

	virtual void quit() override;

	virtual void initBackend() override;

	virtual void logMessage(LogMessageType::Type type, const char *message) override;

private:
	Common::Queue<Common::Event> _eventQueue;
	bool _rescanKeys;

	void doJoyEvent(u32 keysPressed, u32 keysReleased, u32 ndsKey, uint8 svmButton);
};

#endif
