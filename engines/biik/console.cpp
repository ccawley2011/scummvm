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

#include "biik/console.h"
#include "biik/biik.h"
#include "biik/archive.h"
#include "biik/decompress.h"
#include "biik/sound.h"

#include "common/file.h"
#include "common/stream.h"

namespace Biik {

Console::Console(BiikGame *vm) {
	_vm = vm;

	registerCmd("listArchive", WRAP_METHOD(Console, Cmd_listArchive));
	registerCmd("dumpArchive", WRAP_METHOD(Console, Cmd_dumpArchive));
	registerCmd("dumpScript", WRAP_METHOD(Console, Cmd_dumpScript));
	registerCmd("playWaveFile", WRAP_METHOD(Console, Cmd_playWaveFile));
}

bool Console::Cmd_listArchive(int argc, const char** argv) {
	if (argc != 2) {
		debugPrintf("Usage: %s <filename>\n", argv[0]);
		debugPrintf("Lists the contents of the specified archive\n");
		return true;
	}

	BiikArchive archive(_vm->isBigEndian());
	if (!archive.open(argv[1])) {
		debugPrintf("Failed to open archive '%s'\n", argv[1]);
		return true;
	}

	Common::ArchiveMemberList files;
	archive.listMembers(files);

	debugPrintf("Capacity %d\n", files.size());
	for (Common::ArchiveMemberList::iterator it = files.begin(); it != files.end(); ++it)
		debugPrintf("%s\t(%s)\n", (*it)->getName().c_str(), archive.getMemberFileType((*it)->getName()).c_str());

	return true;
}

bool Console::Cmd_dumpArchive(int argc, const char** argv) {
	if (argc != 2) {
		debugPrintf("Usage: %s <filename>\n", argv[0]);
		debugPrintf("Lists the contents of the specified archive\n");
		return true;
	}

	BiikArchive archive(_vm->isBigEndian());
	if (!archive.open(argv[1])) {
		debugPrintf("Failed to open archive '%s'\n", argv[1]);
		return true;
	}

	Common::ArchiveMemberList files;
	archive.listMembers(files);

	for (Common::ArchiveMemberList::iterator it = files.begin(); it != files.end(); ++it) {
		Common::SeekableReadStream *stream = archive.createReadStreamForMember((*it)->getName());
		if (!stream) {
			debugPrintf("Failed to load entry '%s'\n", (*it)->getName().c_str());
			return true;
		}

		Common::String outFile = Common::String::format("dumps/%s/%s", argv[1], (*it)->getName().c_str());
		Common::DumpFile dumpFile;
		dumpFile.open(outFile, true);
		if(!dumpFile.isOpen()) {
			debugPrintf("Failed to create file '%s'\n", outFile.c_str());
			return true;
		}

		dumpFile.writeStream(stream);
		dumpFile.flush();
		dumpFile.close();
	}

	return true;
}

bool Console::Cmd_dumpScript(int argc, const char** argv) {
	if (argc != 3) {
		debugPrintf("Usage: %s <archive> <script>\n", argv[0]);
		debugPrintf("Lists the contents of the specified archive\n");
		return true;
	}

	BiikArchive archive(_vm->isBigEndian());
	archive.open(argv[1]);

	Common::ArchiveMemberList files;
	archive.listMembers(files);

	Common::SeekableReadStream *stream = archive.createReadStreamForMember(argv[2]);
	if (!stream) {
		debugPrintf("Failed to load entry '%s'\n", argv[2]);
		return true;
	}

	/* uint32 page = _vm->isBigEndian() ? stream->readUint32BE() : */ stream->readUint32LE();
	uint32 outSize = _vm->isBigEndian() ? stream->readUint32BE() : stream->readUint32LE();

	Common::SeekableReadStream *compressedStream = Biik::decompressLZW(stream, outSize);
	if (!compressedStream) {
		debugPrintf("Failed to decompress '%s'\n", argv[2]);
		return true;
	}

	Common::String outFile = Common::String::format("dumps/%s/%s.txt", argv[1], argv[2]);
	Common::DumpFile dumpFile;
	dumpFile.open(outFile, true);
	if(!dumpFile.isOpen()) {
		debugPrintf("Failed to create file '%s'\n", outFile.c_str());
		return true;
	}

	dumpFile.writeStream(compressedStream);
	dumpFile.flush();
	dumpFile.close();

	delete compressedStream;

	return true;
}

bool Console::Cmd_playWaveFile(int argc, const char** argv) {
	if (argc != 2 && argc != 3) {
		debugPrintf("Usage: %s <filename> [<path>]\n", argv[0]);
		debugPrintf("Play the specified wave file\n");
		return true;
	}

	if (argc == 3)
		_vm->_sound->setCurrentDir(argv[2]);

	if (!_vm->_sound->playWaveFile(argv[1]))
		debugPrintf("Failed to play wave file '%s'\n", argv[1]);

	return true;
}

} // End of namespace Biik
