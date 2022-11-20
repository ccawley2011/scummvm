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

#include "biik/gui/cursors.h"
#include "biik/biik.h"

#include "common/macresman.h"
#include "common/winexe.h"

#include "graphics/cursor.h"
#include "graphics/cursorman.h"
#include "graphics/maccursor.h"
#include "graphics/macgui/macwindowmanager.h"
#include "graphics/surface.h"
#include "graphics/wincursor.h"

#include "image/rosprite.h"

namespace Biik {

static const byte MOUSECURSOR_RISCOS[] = {
	1,0,0,0,0,0,0,0,0,0,0,0,
	1,1,0,0,0,0,0,0,0,0,0,0,
	1,2,1,0,0,0,0,0,0,0,0,0,
	1,2,2,1,0,0,0,0,0,0,0,0,
	1,2,2,2,1,0,0,0,0,0,0,0,
	1,2,2,2,2,1,0,0,0,0,0,0,
	1,2,2,2,2,2,1,0,0,0,0,0,
	1,2,2,2,2,2,2,1,0,0,0,0,
	1,2,2,2,2,2,2,2,1,0,0,0,
	1,2,2,2,2,2,2,2,2,1,0,0,
	1,2,2,2,2,2,2,2,2,2,1,0,
	1,2,2,2,2,2,1,1,1,1,1,1,
	1,2,2,2,2,2,2,1,0,0,0,0,
	1,2,2,1,1,2,2,1,0,0,0,0,
	1,2,1,0,1,2,2,2,1,0,0,0,
	1,1,0,0,0,1,2,2,1,0,0,0,
	1,0,0,0,0,1,2,2,2,1,0,0,
	0,0,0,0,0,0,1,2,2,1,0,0,
	0,0,0,0,0,0,1,2,2,2,1,0,
	0,0,0,0,0,0,0,1,2,2,1,0,
	0,0,0,0,0,0,0,1,2,2,1,0,
	0,0,0,0,0,0,0,0,1,1,1,0,
};

static const byte cursorPalette[] = {
	0x00, 0x00, 0x00,  // Black / Transparent
	0x00, 0xff, 0xff,  // Cyan
	0x00, 0x00, 0x99,  // Blue
	0xff, 0x00, 0x00   // Red
};

RISCOSCursorManager::RISCOSCursorManager(Image::ROSpriteArea *spriteArea) {
	_spriteArea = spriteArea;

	setDefaultCursor();
}

RISCOSCursorManager::~RISCOSCursorManager() {
}

bool RISCOSCursorManager::loadCursorFromSprite(Common::String name) {
	Image::ROSpriteDecoder *cursor = _spriteArea->getSprite(name);
	if (!cursor)
		return false;

	const Graphics::Surface *surface = cursor->getSurface();
	assert(surface);
	CursorMan.replaceCursor(surface->getPixels(), surface->w, surface->h, 0, 0, 0);
	CursorMan.replaceCursorPalette(cursor->getPalette(), cursor->getPaletteStartIndex(), cursor->getPaletteColorCount());

	return true;
}

void RISCOSCursorManager::setDefaultCursor() {
	CursorMan.showMouse(true);
	CursorMan.replaceCursor(MOUSECURSOR_RISCOS, 12, 22, 0, 0, 0);
	CursorMan.replaceCursorPalette(cursorPalette, 0, 4);
}

void RISCOSCursorManager::setPtrClickCursor() {
	CursorMan.showMouse(true);
	// TODO: Support the blinking cursor from later games
	if (!loadCursorFromSprite("ptr_click")) {
		setDefaultCursor();
	}
}

void RISCOSCursorManager::setPtrNoTouch() {
	CursorMan.showMouse(false);
	if (loadCursorFromSprite("ptr_notouch")) {
		CursorMan.showMouse(true);
	}
}

WindowsCursorManager::WindowsCursorManager(Common::WinResources *exe) {
	_ptr_default = Graphics::makeDefaultWinCursor();
	_ptr_click = Graphics::WinCursorGroup::createCursorGroup(exe, 105);
	_ptr_notouch = Graphics::WinCursorGroup::createCursorGroup(exe, 106);

	setDefaultCursor();
}

WindowsCursorManager::~WindowsCursorManager() {
	delete _ptr_click;
	delete _ptr_notouch;
	delete _ptr_default;
}

void WindowsCursorManager::setDefaultCursor() {
	CursorMan.showMouse(true);
	CursorMan.replaceCursor(_ptr_default);
}

void WindowsCursorManager::setPtrClickCursor() {
	if (_ptr_click) {
		CursorMan.showMouse(true);
		Graphics::Cursor *cursor = _ptr_click->cursors[0].cursor;
		CursorMan.replaceCursor(cursor);
	} else {
		setDefaultCursor();
	}
}

void WindowsCursorManager::setPtrNoTouch() {
	if (_ptr_notouch) {
		CursorMan.showMouse(true);
		Graphics::Cursor *cursor = _ptr_notouch->cursors[0].cursor;
		CursorMan.replaceCursor(cursor);
	} else {
		CursorMan.showMouse(false);
	}
}

MacCursorManager::MacCursorManager(Graphics::MacWindowManager *wm, Common::MacResManager *resMan) {
	_ptr_click = loadCursor(resMan, 128);
	_ptr_notouch = loadCursor(resMan, 129);
	_wm = wm;

	setDefaultCursor();
}

MacCursorManager::~MacCursorManager() {
	delete _ptr_click;
	delete _ptr_notouch;
}

Graphics::MacCursor *MacCursorManager::loadCursor(Common::MacResManager *resMan, uint16 id) {
	Common::SeekableReadStream *stream = resMan->getResource(MKTAG('C', 'U', 'R', 'S'), id);
	assert(stream);

	Graphics::MacCursor *macCursor = new Graphics::MacCursor();
	if (!macCursor->readFromStream(*stream))
		error("Could not parse Mac cursor");

	delete stream;

	return macCursor;
}

void MacCursorManager::setDefaultCursor() {
	_wm->replaceCursor(Graphics::kMacCursorArrow);
}

void MacCursorManager::setPtrClickCursor() {
	_wm->replaceCursor(Graphics::kMacCursorCustom, _ptr_click);
}

void MacCursorManager::setPtrNoTouch() {
	_wm->replaceCursor(Graphics::kMacCursorCustom, _ptr_notouch);
}

}
