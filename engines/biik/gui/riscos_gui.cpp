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

#include "biik/gui/riscos_gui.h"
#include "biik/gui/cursors.h"
#include "biik/gui/element.h"
#include "biik/gui/riscos_template.h"
#include "biik/biik.h"

#include "common/file.h"

#include "graphics/font.h"
#include "graphics/fontman.h"
#include "graphics/managed_surface.h"
#include "graphics/screen.h"
#include "graphics/surface.h"

#include "image/rosprite.h"

#include "engines/util.h"

namespace Biik {

RISCOSGUI::RISCOSGUI(BiikGame *vm)
	: BiikGUI(vm),
	  _spriteArea(nullptr) {
}

RISCOSGUI::~RISCOSGUI() {
	delete _spriteArea;
}

Common::Error RISCOSGUI::init() {
	Common::File spriteFile;
	const char *spriteName = _vm->getFileName(GAME_SPRITEFILE);
	if (!spriteName || !spriteFile.open(spriteName)) {
		warning("Failed to load file '%s'", spriteName);
		return Common::kNoGameDataFoundError;
	}
	_spriteArea = new Image::ROSpriteArea();
	if (!_spriteArea->loadStream(spriteFile)) {
		warning("Failed to load file '%s'", spriteName);
		return Common::kNoGameDataFoundError;
	}

	_cursorMan = new RISCOSCursorManager(_spriteArea);

	Template sketchpad;
	const char *templateName = _vm->getFileName(GAME_TEMPLATEFILE);
	if (!templateName || !sketchpad.open(templateName, "sketchpad")) {
		warning("Failed to load file '%s'", templateName);
		return Common::kNoGameDataFoundError;
	}

	const Template::WindowDef *windowDef = sketchpad.getWindowDef();
	int width = (windowDef->workAreaMaxX - windowDef->workAreaMinX) / 2;
	int height = (windowDef->workAreaMaxY - windowDef->workAreaMinY) / 2;

	// The width and height usually ends up as 638 * 438
	initGraphics(width, height);
	_screen = new Graphics::Screen();
	_screen->setPalette(Image::riscos_palette_256, 0, 256);
	_screen->clear(Image::riscos_palette_16_to_256[windowDef->workAreaBgColour]);

	const Graphics::Font *font = FontMan.getFontByUsage(Graphics::FontManager::kBigGUIFont);

	int id = 0;
	for (Common::Array<Template::IconDef>::const_iterator it = sketchpad.getIconDefs().begin(); it != sketchpad.getIconDefs().end(); it++) {

		Common::Rect rect(it->minx / 2, height - (it->maxy / 2), it->maxx / 2, height - (it->miny / 2));
		Element *element = new Element(*_screen, rect, id++);

		if (!(it->flags & Template::ICON_IS_DELETED))
			element->setSurface(_spriteArea->getSprite("logo"));
		element->setText(font, Common::String::format("%d", element->getID()));
		debugC(kDebugGUI, "Icon %02d: %s %s", element->getID(), it->iconData.name.c_str(), it->iconData.valid.c_str());

		_elements.push_back(element);
		element->update();
	}

	return Common::kNoError;
}

}
