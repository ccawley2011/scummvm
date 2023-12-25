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

#include "noot/book.h"

#include "common/archive.h"
#include "common/stream.h"

namespace Noot {

Book::Book() :
	_stream(nullptr),
	_index(nullptr),
	_indexCount(0) {
	memset(&_header, 0, sizeof(_header));
	memset(_positions, 0, sizeof(_positions));
}

bool Book::open(const Common::Path &filename) {
	close();

	_stream = SearchMan.createReadStreamForMember(filename);

	if (!_stream)
		return false;

	if (!readHeader()) {
		close();
		return false;
	}

	_stream->seek(_header.index);

	uint32 type = _stream->readUint32LE();
	uint32 size = _stream->readUint32LE();
	if (type == kResourceIndex)
		readIndex(size);

	rewind();

	return true;
}

void Book::close() {
	delete[] _index;
	_index = nullptr;
	_indexCount = 0;

	delete[] _header.palette;
	memset(&_header, 0, sizeof(_header));

	delete _stream;
	_stream = nullptr;
}

void Book::rewind() {
	_stream->seek(_header.start);
	memset(_positions, 0, sizeof(_positions));
}

Common::SeekableReadStream *Book::loadResource(ResourceType resType, uint32 pos) {
	assert(resType > 0 && resType < kResourceMax);

	if (_positions[resType] >= pos)
		rewind();

	while (!_stream->eos() && !_stream->err()) {
		uint32 type = _stream->readUint32LE();
		uint32 size = _stream->readUint32LE();

		if (_stream->eos() || _stream->err()) {
			break;
		} else if (type < kResourceMax) {
			_positions[type] += 1;

			if (type == resType && _positions[type] == pos) {
				return _stream->readStream(size - 8);
			} else {
				_stream->skip(size - 8);
			}
		} else {
			_stream->skip(size - 8);
		}
	}

	return nullptr;
}

bool Book::readHeader() {
	_header.magic    = _stream->readUint32LE();
	if (_header.magic != MKTAG('T', 'O', 'O', 'N'))
		return false;
	_header.start    = _stream->readUint32LE();
	_header.unknown1 = _stream->readUint32LE();
	_header.doctype  = _stream->readSint32LE();
	_header.unknown2 = _stream->readUint32LE();
	_stream->read(_header.desc, 48);
	_header.index    = _stream->readUint32LE();
	_header.unknown3 = _stream->readUint32LE();
	_header.mode     = _stream->readUint32LE();
	_header.xdiv     = _stream->readUint32LE();
	_header.ydiv     = _stream->readUint32LE();
	_header.colours  = _stream->readUint32LE();
	_header.palette  = new byte[_header.colours * 4];
	_stream->read(_header.palette, _header.colours * 4);

	return true;
}

bool Book::readIndex(uint32 size) {
	uint32 count = _stream->readUint32LE();
	if ((count * 8) + 4 > size)
		return false;

	_indexCount = count;
	_index = new ScriptIndex[count];

	for (uint32 i = 0; i < _indexCount; i++) {
		_index[i].id     = _stream->readUint32LE();
		_index[i].offset = _stream->readUint32LE();
	}

	return true;
}

const char *Book::BookHeader::getTypeStr() const {
	switch (doctype) {
	case 0: return "Normal";
	case -1: return "Demo";
	case -2: return "Granny's Garden";
	default: return "Unknown";
	}
}

} // End of namespace Noot
