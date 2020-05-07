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

#ifndef BIIK_GUI_CURSORS_H
#define BIIK_GUI_CURSORS_H

#include "common/str.h"

namespace Common {
class WinResources;
class MacResManager;
}

namespace Graphics {
class Cursor;
class MacCursor;
class MacWindowManager;
class WinCursorGroup;
}

namespace Image {
class ROSpriteArea;
}

namespace Biik {

class CursorManager {
public:
	virtual ~CursorManager() {}

	virtual void setDefaultCursor() = 0;
	virtual void setPtrClickCursor() = 0;
	virtual void setPtrNoTouch() = 0;
};

class RISCOSCursorManager : public CursorManager {
public:
	RISCOSCursorManager(Image::ROSpriteArea *spriteArea);
	virtual ~RISCOSCursorManager();

	virtual void setDefaultCursor();
	virtual void setPtrClickCursor();
	virtual void setPtrNoTouch();

private:
	Image::ROSpriteArea *_spriteArea;
	bool loadCursorFromSprite(Common::String name);
};

class WindowsCursorManager : public CursorManager {
public:
	WindowsCursorManager(Common::WinResources *exe);
	virtual ~WindowsCursorManager();

	virtual void setDefaultCursor();
	virtual void setPtrClickCursor();
	virtual void setPtrNoTouch();

private:
	Graphics::Cursor *_ptr_default;
	Graphics::WinCursorGroup *_ptr_click, *_ptr_notouch;
};

class MacCursorManager : public CursorManager {
public:
	MacCursorManager(Graphics::MacWindowManager *wm, Common::MacResManager *resMan);
	virtual ~MacCursorManager();

	virtual void setDefaultCursor();
	virtual void setPtrClickCursor();
	virtual void setPtrNoTouch();

private:
	Graphics::MacWindowManager *_wm;
	Graphics::MacCursor *_ptr_click, *_ptr_notouch;
	Graphics::MacCursor *loadCursor(Common::MacResManager *resMan, uint16 id);
};

} // End of namespace Biik

#endif
