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

#ifndef MOHAWK_MYST_CONSOLE_H
#define MOHAWK_MYST_CONSOLE_H

#include "gui/debugger.h"

namespace Mohawk {

class MohawkEngine_Myst;

class MystConsole : public GUI::Debugger {
public:
	explicit MystConsole(MohawkEngine_Myst *vm);
	~MystConsole() override;

private:
	MohawkEngine_Myst *_vm;

	bool Cmd_ChangeCard(int argc, const char **argv);
	bool Cmd_CurCard(int argc, const char **argv);
	bool Cmd_Var(int argc, const char **argv);
	bool Cmd_DrawImage(int argc, const char **argv);
	bool Cmd_DrawRect(int argc, const char **argv);
	bool Cmd_SetResourceEnable(int argc, const char **argv);
	bool Cmd_CurStack(int argc, const char **argv);
	bool Cmd_ChangeStack(int argc, const char **argv);
	bool Cmd_PlaySound(int argc, const char **argv);
	bool Cmd_StopSound(int argc, const char **argv);
	bool Cmd_PlayMovie(int argc, const char **argv);
	bool Cmd_DisableInitOpcodes(int argc, const char **argv);
	bool Cmd_Cache(int argc, const char **argv);
	bool Cmd_Resources(int argc, const char **argv);
	bool Cmd_QuickTest(int argc, const char **argv);
};

} // End of namespace Mohawk

#endif
