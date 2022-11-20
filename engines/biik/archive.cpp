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

#include "biik/archive.h"

#include "common/debug.h"
#include "common/substream.h"

namespace Biik {

BiikArchive::BiikArchive(bool bigEndian) : Common::Archive() {
	_bigEndian = bigEndian;
	_stream = nullptr;
}

BiikArchive::~BiikArchive() {
	close();
}

bool BiikArchive::open(const Common::String &filename) {
	close();

	_stream = SearchMan.createReadStreamForMember(filename);
	if (!_stream)
		return false;

	Common::SeekableSubReadStreamEndian subStream(_stream, 0, _stream->size(), _bigEndian, DisposeAfterUse::NO);

	if (subStream.readUint32BE() != MKTAG('B', 'I', 'I', 'K')) {
		return false;
	}
	if (subStream.readUint32BE() != MKTAG('-', 'D', 'J', 'C')) {
		return false;
	}

	uint32 fileHeaderSize = subStream.readUint32();
	if (fileHeaderSize < 0x18) {
		return false;
	}
	uint32 version = subStream.readUint32();
	uint32 gameId = subStream.readUint32();
	uint32 indexOffset = subStream.readUint32();

	debug(1, "Opening archive %s: version %x, gameId %x", filename.c_str(), version, gameId);

	subStream.seek(indexOffset);

	subStream.skip(4); // Skip unknown value
	uint32 indexSize = subStream.readUint32();
	uint32 indexHeaderSize = subStream.readUint32();
	subStream.skip(indexHeaderSize - 12); // Skip "File index" string

	while (subStream.pos() < (int32)(indexOffset + indexSize) && !subStream.eos()) {
		FileEntry entry;

		uint32 nextOffset = subStream.pos() + subStream.readUint32();
		uint32 headerOffset = subStream.readUint32();
		Common::String name = subStream.readString();

		subStream.seek(headerOffset);
		entry.type = subStream.readUint32();
		uint32 fileSize = subStream.readUint32();
		uint32 headerSize = subStream.readUint32();
		Common::String headerName = subStream.readString();

		entry.size = fileSize - headerSize;
		entry.offset = headerOffset + headerSize;

		if (name != headerName)
			warning("Filenames in the index (%s) and the header (%s) don't match", name.c_str(), headerName.c_str());

		subStream.seek(nextOffset);
		_map[name] = entry;
	}

	return true;
}

void BiikArchive::close() {
	delete _stream; _stream = nullptr;
	_map.clear();
}

bool BiikArchive::hasFile(const Common::Path &path) const {
	Common::String name = path.toString();
	return _map.contains(name);
}

int BiikArchive::listMembers(Common::ArchiveMemberList &list) const {
	for (FileMap::const_iterator it = _map.begin(); it != _map.end(); it++)
		list.push_back(getMember(it->_key));

	return _map.size();
}

const Common::ArchiveMemberPtr BiikArchive::getMember(const Common::Path &path) const {
	Common::String name = path.toString();
	return Common::ArchiveMemberPtr(new Common::GenericArchiveMember(name, this));
}

Common::SeekableReadStream *BiikArchive::createReadStreamForMember(const Common::Path &path) const {
	Common::String name = path.toString();
	if (!_stream || !_map.contains(name))
		return nullptr;

	const FileEntry &entry = _map[name];

	return new Common::SeekableSubReadStream(_stream, entry.offset, entry.offset + entry.size, DisposeAfterUse::YES);
}

Common::String BiikArchive::getMemberFileType(const Common::Path &path) const {
	Common::String name = path.toString();
	if (!_stream || !_map.contains(name))
		return nullptr;

	const FileEntry &entry = _map[name];

	switch (entry.type) {
	case ENTRY_TYPE_GRAPHIC: return "Graphics";
	case ENTRY_TYPE_TRACKER: return "Tracker";
	case ENTRY_TYPE_SCRIPT:  return "Script";
	case ENTRY_TYPE_DRAW:    return "Draw";
	case ENTRY_TYPE_COORDS:  return "Co-ordinates";
	}

	return Common::String::format("unknown (%d)", entry.type);
}

} // End of namespace Biik
