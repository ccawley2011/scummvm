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

#ifndef BIIK_GUI_H
#define BIIK_GUI_H

#include "common/array.h"
#include "common/error.h"
#include "common/events.h"
#include "common/rect.h"

namespace Graphics {
class Screen;
}

namespace Biik {

class BiikGame;
class CursorManager;
class Element;

class BiikGUI {
public:
	BiikGUI(BiikGame *vm);
	virtual ~BiikGUI();

	virtual Common::Error init() = 0;
	virtual Common::Error run();

	virtual void updateScreen();
	virtual void onMouseClick(bool isRight, const Common::Point &pos);
	virtual bool processEvent(Common::Event &event) { return false; }

protected:
	BiikGame *_vm;
	CursorManager *_cursorMan;
	Graphics::Screen *_screen;
	Common::Array<Element *> _elements;
	bool _quit;
};

} // End of namespace Biik

#endif
