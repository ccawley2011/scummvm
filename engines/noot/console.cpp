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

#include "noot/console.h"
#include "noot/noot.h"
#include "noot/book.h"

namespace Noot {

Console::Console(NootEngine *engine) : GUI::Debugger(), _engine(engine) {
	registerCmd("about",    WRAP_METHOD(Console, Cmd_about));
	registerCmd("playAnim", WRAP_METHOD(Console, Cmd_playAnim));
}

Console::~Console() {
}

bool Console::Cmd_about(int argc, const char **argv) {
	const Book *book = _engine->getBook();
	if (!book) {
		debugPrintf("No book is currently loaded\n");
		return true;
	}

	const Book::BookHeader *header = book->getHeader();
	debugPrintf("Offset of first chunk: 0x%x\n", header->start);
	debugPrintf("Unknown: %d\n", header->unknown1);
	debugPrintf("Document type: %d (%s)\n", header->doctype, header->getTypeStr());
	debugPrintf("Unknown: %d\n", header->unknown2);
	debugPrintf("Created using: %s\n", Common::String(header->desc, 48).c_str());
	debugPrintf("Offset of index chunk: 0x%x\n", header->index);
	debugPrintf("Unknown: %d\n", header->unknown3);
	debugPrintf("Mode: %d\n", header->mode);
	debugPrintf("X div: %d\n", header->xdiv);
	debugPrintf("Y div: %d\n", header->ydiv);
	debugPrintf("Number of palette entries: %d\n", header->colours);

	return true;
}

bool Console::Cmd_playAnim(int argc, const char **argv) {
	if (argc != 2) {
		debugPrintf("Syntax: playAnim <num>\n");
		return true;
	}

	if (!_engine->loadAnimation(atoi(argv[1]))) {
		debugPrintf("Unable to load animation %d\n", atoi(argv[1]));
		return true;
	}

	return false;
}

} // End of namespace Noot
