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

#ifndef MOHAWK_RIVEN_CONSOLE_H
#define MOHAWK_RIVEN_CONSOLE_H

#include "gui/debugger.h"

namespace Mohawk {

class MohawkEngine_Riven;

class RivenConsole : public GUI::Debugger {
public:
	explicit RivenConsole(MohawkEngine_Riven *vm);
	~RivenConsole() override;

private:
	MohawkEngine_Riven *_vm;

	bool Cmd_ChangeCard(int argc, const char **argv);
	bool Cmd_CurCard(int argc, const char **argv);
	bool Cmd_Var(int argc, const char **argv);
	bool Cmd_PlaySound(int argc, const char **argv);
	bool Cmd_PlaySLST(int argc, const char **argv);
	bool Cmd_StopSound(int argc, const char **argv);
	bool Cmd_CurStack(int argc, const char **argv);
	bool Cmd_ChangeStack(int argc, const char **argv);
	bool Cmd_Hotspots(int argc, const char **argv);
	bool Cmd_ZipMode(int argc, const char **argv);
	bool Cmd_DumpCard(int argc, const char **argv);
	bool Cmd_DumpStack(int argc, const char **argv);
	bool Cmd_DumpScript(int argc, const char **argv);
	bool Cmd_ListZipCards(int argc, const char **argv);
	bool Cmd_GetRMAP(int argc, const char **argv);
	bool Cmd_Combos(int argc, const char **argv);
	bool Cmd_SliderState(int argc, const char **argv);
	bool Cmd_QuickTest(int argc, const char **argv);
};

} // End of namespace Mohawk

#endif
