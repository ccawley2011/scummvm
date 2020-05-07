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

#include "biik/script.h"
#include "biik/biik.h"
#include "biik/decompress.h"

#include "common/archive.h"
#include "common/stream.h"

namespace Biik {

Script::Script(BiikGame *vm)
	: _vm(vm) {
}

Script::~Script() {
}

bool Script::load(Common::SeekableReadStream *stream) {
	_script.clear();

	uint32 outSize;
	if (_vm->getPlatform() == Common::kPlatformMacintosh) {
		/* uint32 unknown = */ stream->readUint32BE();
		outSize = stream->readUint32BE();
	} else {
		/* uint32 unknown = */ stream->readUint32LE();
		outSize = stream->readUint32LE();
	}

	Common::SeekableReadStream *decompressed = Biik::decompressLZW(stream, outSize);
	if (!decompressed)
		return false;

	while (!decompressed->eos()) {
		Common::String line = decompressed->readLine(false);
		if (line.empty() || line.firstChar() == '|' || !line.contains(':'))
			continue;

		size_t colon = line.find(':');
		Common::String id = Common::String(line.c_str(), colon);
		Common::String commands = Common::String(line.c_str() + colon + 1, line.size() - colon - 1);

		_script[id] = commands;
	}

	delete decompressed;
	return true;
}

bool Script::load(Common::Archive *archive, Common::String name) {
	_script.clear();

	Common::SeekableReadStream *stream = archive->createReadStreamForMember(name);
	if (!stream)
		return false;

	return load(stream);
}

}
