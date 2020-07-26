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

#ifndef BIIK_RESMAN_H
#define BIIK_RESMAN_H

#include "common/hash-str.h"
#include "common/str.h"
#include "common/str-array.h"

namespace Common {
class Archive;
class SeekableReadStream;
}

namespace Biik {

class BiikGame;

class Script {
public:

public:
	Script(BiikGame *vm);
	~Script();

	bool load(Common::SeekableReadStream *stream);
	bool load(Common::Archive *archive, Common::String name);

	void run(const Common::String id);

	void command(const Common::String &command);

private:
	BiikGame *_vm;
	Common::StringMap _script;

	Common::StringArray _stringVars;
	Common::Array<int16> _intVars;

	typedef void (Script::*Command)(const Common::String &command);

	struct CommandListEntry {
		const char *prefix;
		Command func;
	};

	static const CommandListEntry _commandList[];

	void cmd_Call(const Common::String &command);
	void cmd_SetInt(const Common::String &command);
	void cmd_SetString(const Common::String &command);
};

}

#endif
