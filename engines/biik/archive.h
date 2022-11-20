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

#ifndef BIIK_ARCHIVE_H
#define BIIK_ARCHIVE_H

#include "common/archive.h"
#include "common/hashmap.h"
#include "common/hash-str.h"

namespace Biik {

class BiikArchive : public Common::Archive {
public:
	BiikArchive(bool bigEndian = false);
	~BiikArchive() override;

	bool open(const Common::String &filename);
	void close();
	bool isOpen() const { return _stream != nullptr; }

	// Common::Archive API implementation
	bool hasFile(const Common::Path &path) const override;
	int listMembers(Common::ArchiveMemberList &list) const override;
	const Common::ArchiveMemberPtr getMember(const Common::Path &path) const override;
	Common::SeekableReadStream *createReadStreamForMember(const Common::Path &path) const override;

	Common::String getMemberFileType(const Common::Path &path) const;

private:
	struct FileEntry {
		uint32 size;
		uint32 offset;
		uint32 type;
	};

	Common::SeekableReadStream *_stream;

	typedef Common::HashMap<Common::String, FileEntry, Common::IgnoreCase_Hash, Common::IgnoreCase_EqualTo> FileMap;
	FileMap _map;

	bool _bigEndian;
};

enum {
	GAME_ID_DINODEMO  = 1,
	GAME_ID_MOUSE     = 3,
	GAME_ID_FLOSSY    = 5,
	GAME_ID_DINOSAUR  = 6,
	GAME_ID_BETSI     = 8,
	GAME_ID_GUARDIANS = 13,
	GAME_ID_FIFI      = 18,
	GAME_ID_DARRYL    = 255
};

enum {
	ENTRY_TYPE_GRAPHIC = 1,
	ENTRY_TYPE_TRACKER = 2,
	ENTRY_TYPE_SCRIPT  = 9,
	ENTRY_TYPE_DRAW    = 11,
	ENTRY_TYPE_COORDS  = 15
};

} // End of namespace Biik

#endif
