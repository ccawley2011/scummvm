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

#ifndef MOHAWK_LIVINGBOOKS_CONSOLE_H
#define MOHAWK_LIVINGBOOKS_CONSOLE_H

#include "gui/debugger.h"

namespace Mohawk {

class MohawkEngine_LivingBooks;

class LivingBooksConsole : public GUI::Debugger {
public:
	explicit LivingBooksConsole(MohawkEngine_LivingBooks *vm);
	~LivingBooksConsole() override;

private:
	MohawkEngine_LivingBooks *_vm;

	bool Cmd_PlaySound(int argc, const char **argv);
	bool Cmd_StopSound(int argc, const char **argv);
	bool Cmd_ListSounds(int argc, const char **argv);
	bool Cmd_DrawImage(int argc, const char **argv);
	bool Cmd_ChangePage(int argc, const char **argv);
	bool Cmd_ChangeCursor(int argc, const char **argv);
};

} // End of namespace Mohawk

#endif
