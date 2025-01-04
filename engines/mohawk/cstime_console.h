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

#ifndef MOHAWK_CSTIME_CONSOLE_H
#define MOHAWK_CSTIME_CONSOLE_H

#include "gui/debugger.h"

namespace Mohawk {

class MohawkEngine_CSTime;

class CSTimeConsole : public GUI::Debugger {
public:
	CSTimeConsole(MohawkEngine_CSTime *vm);
	~CSTimeConsole(void) override;

private:
	MohawkEngine_CSTime *_vm;

	bool Cmd_PlaySound(int argc, const char **argv);
	bool Cmd_StopSound(int argc, const char **argv);
	bool Cmd_DrawImage(int argc, const char **argv);
	bool Cmd_DrawSubimage(int argc, const char **argv);
	bool Cmd_ChangeCase(int argc, const char **argv);
	bool Cmd_ChangeScene(int argc, const char **argv);
	bool Cmd_CaseVariable(int argc, const char **argv);
	bool Cmd_InvItem(int argc, const char **argv);
};

} // End of namespace Mohawk

#endif
