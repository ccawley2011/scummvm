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

#include "biik/gui/windows_gui.h"
#include "biik/gui/cursors.h"
#include "biik/biik.h"
#include "biik/detection.h"

#include "common/archive.h"
#include "common/fs.h"
#include "common/stream.h"
#include "common/system.h"
#include "common/winexe.h"

#include "engines/util.h"

#include "graphics/palette.h"
#include "graphics/screen.h"

#include "image/rosprite.h"

namespace Biik {

WindowsGUI::WindowsGUI(BiikGame *vm)
	: BiikGUI(vm),
	  _4mat_logo(nullptr),
	  _click_here(nullptr) {
}

WindowsGUI::~WindowsGUI() {
	delete _4mat_logo;
	delete _click_here;
}

Common::Error WindowsGUI::init() {
	const char *exeName = _vm->getFileName(GAME_EXEFILE);
	Common::WinResources *exe = Common::WinResources::createFromEXE(exeName);
	if (!exe) {
		return Common::kNoGameDataFoundError;
	}

	_cursorMan = new WindowsCursorManager(exe);
	_4mat_logo = loadGUISprite(exe, 102);
	_click_here = loadGUISprite(exe, 103);

	delete exe;

	initGraphics(640, 480);
	_screen = new Graphics::Screen();
	_screen->setPalette(Image::riscos_palette_256, 0, 256);
	_screen->clear(252);

	if (_4mat_logo) {
		Image::ROSpriteDecoder *sprite = _4mat_logo->getSprite(0);
		Graphics::ManagedSurface surface(sprite->getSurface()->w, sprite->getSurface()->h);
		surface.blitFrom(*sprite->getSurface());
		if (sprite->hasPalette())
			surface.setPalette(sprite->getPalette(), sprite->getPaletteStartIndex(), sprite->getPaletteColorCount());

		Common::Rect rect(_screen->w / 2, 0, _screen->w, _screen->h / 2);
		_screen->transBlitFrom(surface, surface.getBounds(), rect, 0, false, 0, 0xff, sprite->getMask(), true);
	} else {
		warning("Unable to load sprite '4mat_logo'");
	}

	if (_click_here) {
		Image::ROSpriteDecoder *sprite = _click_here->getSprite(0);
		Graphics::ManagedSurface surface(sprite->getSurface()->w, sprite->getSurface()->h);
		surface.blitFrom(*sprite->getSurface());
		if (sprite->hasPalette())
			surface.setPalette(sprite->getPalette(), sprite->getPaletteStartIndex(), sprite->getPaletteColorCount());

		Common::Rect rect(_screen->w / 2, 0, _screen->w, _screen->h / 2);
		_screen->transBlitFrom(surface, surface.getBounds(), rect, 0, false, 0, 0xff, sprite->getMask(), true);
	} else {
		warning("Unable to load sprite 'click_here'");
	}

	return Common::kNoError;
}

Image::ROSpriteArea *WindowsGUI::loadGUISprite(Common::WinResources *exe, int id) {
	Common::SeekableReadStream *stream = exe->getResource(Common::WinResourceID("SPRITE"), id);
	if (!stream)
		return nullptr;

	Image::ROSpriteArea *area = new Image::ROSpriteArea();
	if (area->loadStream(*stream)) {
		delete stream;
		return area;
	}

	delete stream;
	delete area;
	return nullptr;
}

}
