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

#ifndef NOOT_BOOK_H
#define NOOT_BOOK_H

#include "common/scummsys.h"

namespace Common {
class Path;
class SeekableReadStream;
}

namespace Noot {

class Book {
public:
	Book();
	~Book() { close(); }

	bool open(const Common::Path &filename);
	void close();

	enum ResourceType : uint {
		kResourceAnimation = 1,
		kResourceMusic,
		kResourceScript,
		kResourceIndex,

		kResourceMax
	};

	void rewind();
	Common::SeekableReadStream *loadResource(ResourceType resType, uint32 pos);

	struct BookHeader {
		uint32 magic;
		uint32 start;
		uint32 unknown1;
		int32 doctype;
		uint32 unknown2;
		char desc[48];
		uint32 index;
		uint32 unknown3;
		uint32 mode;
		uint32 xdiv;
		uint32 ydiv;
		uint32 colours;
		uint8 *palette;

		const char *getTypeStr() const;
	};

	const BookHeader *getHeader() const {
		return &_header;
	}

	struct ScriptIndex {
		uint32 id;
		uint32 offset;
	};

	const ScriptIndex *getScriptIndex(uint32 &count) const {
		count = _indexCount;
		return _index;
	}

private:
	bool readHeader();
	bool readIndex(uint32 size);

	Common::SeekableReadStream *_stream;
	BookHeader _header;
	ScriptIndex *_index;
	uint32 _indexCount;
	uint _positions[kResourceMax];
};

} // End of namespace Noot

#endif // NOOT_BOOK_H
