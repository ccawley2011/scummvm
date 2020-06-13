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

#include "biik/gui/riscos_template.h"

#include "common/file.h"
#include "common/stream.h"
#include "common/textconsole.h"

namespace Biik {

Template::Template() {
	destroy();
}

Template::~Template() {
	destroy();
}

void Template::destroy() {
	_index.clear();
	_id = -1;
}

bool Template::open(const Common::String &fileName, Common::String name) {
	if (fileName.empty())
		return false;

	Common::File file;

	if (!file.open(fileName)) {
		return false;
	}

	return open(&file, name);
}

bool Template::open(Common::SeekableReadStream *stream, Common::String name) {
	destroy();

	if (!stream)
		return false;

	stream->skip(16);

	while (!stream->eos() && loadIndex(stream));

	for (uint j = 0; j < _index.size(); j++) {
		if (Common::String(_index[j].identifier) == name) {
			_id = j;
			break;
		}
	}

	if (_id == -1) {
		warning("Could not find template '%s'", name.c_str());
		return false;
	}

	stream->seek(_index[_id].offset);
	_windowDef = loadWindowDef(stream);

	return true;
}

bool Template::loadIndex(Common::SeekableReadStream *stream) {
	Index index;
	index.offset = stream->readUint32LE();
	if (index.offset == 0)
		return false;
	index.size = stream->readUint32LE();
	index.entryType = stream->readUint32LE();
	stream->read(index.identifier, 12);
	for (int i = 0; i < 12; i++) {
		if (index.identifier[i] == 0xD)
			index.identifier[i] = 0;
	}

	_index.push_back(index);
	return true;
}

Template::WindowDef Template::loadWindowDef(Common::SeekableReadStream *stream) {
	WindowDef windowDef;

	windowDef.minx = stream->readUint32LE();
	windowDef.miny = stream->readUint32LE();
	windowDef.maxx = stream->readUint32LE();
	windowDef.maxy = stream->readUint32LE();
	windowDef.scrollx = stream->readUint32LE();
	windowDef.scrolly = stream->readUint32LE();
	windowDef.handleBehind = stream->readUint32LE();
	windowDef.windowFlags = stream->readUint32LE();
	windowDef.titleFgColour = stream->readByte();
	windowDef.titleBgColour = stream->readByte();
	windowDef.workAreaFgColour = stream->readByte();
	windowDef.workAreaBgColour = stream->readByte();
	windowDef.scrollOuterColour = stream->readByte();
	windowDef.scrollInnerColour = stream->readByte();
	windowDef.titleFocusBgColour = stream->readByte();
	windowDef.reserved = stream->readByte();
	windowDef.workAreaMinX = stream->readUint32LE();
	windowDef.workAreaMinY = stream->readUint32LE();
	windowDef.workAreaMaxX = stream->readUint32LE();
	windowDef.workAreaMaxY = stream->readUint32LE();
	windowDef.titleFlags = stream->readUint32LE();
	windowDef.workAreaFlags = stream->readUint32LE();
	windowDef.spriteAreaControlBlock = stream->readUint32LE();
	windowDef.minWidth = stream->readUint16LE();
	windowDef.minHeight = stream->readUint16LE();
	windowDef.titleData = readIndirectedData(stream, _windowDef.titleFlags);
	windowDef.numIcons = stream->readUint32LE();

	for (uint32 i = 0; i < windowDef.numIcons; i++)
		windowDef.iconDefs.push_back(loadIconDef(stream));

	return windowDef;
}

Template::IconDef Template::loadIconDef(Common::SeekableReadStream *stream) {
	IconDef iconDef;

	iconDef.minx = stream->readUint32LE();
	iconDef.miny = stream->readUint32LE();
	iconDef.maxx = stream->readUint32LE();
	iconDef.maxy = stream->readUint32LE();
	iconDef.flags = stream->readUint32LE();
	iconDef.iconData = readIndirectedData(stream, iconDef.flags);

	return iconDef;
}

Template::IndirectedData Template::readIndirectedData(Common::SeekableReadStream *stream, uint32 flags) {
	bool hasText = (flags & Template::ICON_CONTAINS_SPRITE);
	bool hasSprite = (flags & Template::ICON_CONTAINS_SPRITE);
	bool isIndirected = (flags & Template::ICON_IS_INDIRECTED);

	IndirectedData data;

	if (isIndirected) {
		data.indirect.nameptr = stream->readUint32LE();
		data.indirect.validptr = stream->readUint32LE();
		data.indirect.length = stream->readUint32LE();

		if (hasText || (hasSprite && data.indirect.length > 0)) {
			data.name = readString(stream, _index[_id].offset + data.indirect.nameptr, data.indirect.length);
		}
		if (hasText && data.indirect.validptr > -1) {
			data.valid = readString(stream, _index[_id].offset + data.indirect.validptr);
		}
	} else {
		stream->read(data.data, 12);
		for (uint i = 0; i < 12; i++) {
			if (data.data[i] == 0xD)
				data.data[i] = 0;
		}
		data.name = data.data;
	}
	return data;
}

Common::String Template::readString(Common::SeekableReadStream *stream, int32 offset, size_t bufLength) {
	char *temp = new char[bufLength + 1];
	int32 oldpos = stream->pos();
	Common::String str;

	memset(temp, 0, bufLength + 1);
	stream->seek(offset);
	stream->read(temp, bufLength);

	for (uint i = 0; i < bufLength; i++) {
		if (temp[i] == 0xD)
			temp[i] = 0;
	}

	str = temp;
	delete[] temp;
	stream->seek(oldpos);

	return str;
}

Common::String Template::readString(Common::SeekableReadStream *stream, int32 offset) {
	int32 oldpos = stream->pos();

	stream->seek(offset);
	Common::String str = stream->readLine();
	stream->seek(oldpos);

	return str;
}

}
