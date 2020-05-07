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

#include "biik/gui/gui.h"
#include "biik/gui/cursors.h"
#include "biik/gui/element.h"
#include "biik/biik.h"

#include "common/system.h"

#include "graphics/managed_surface.h"
#include "graphics/screen.h"
#include "graphics/surface.h"

namespace Biik {

BiikGUI::BiikGUI(BiikGame *vm)
	: _vm(vm),
	  _cursorMan(nullptr),
	  _screen(nullptr),
	  _quit(false) {
}

BiikGUI::~BiikGUI() {
	for (ElementArray::iterator it = _elements.begin(); it != _elements.end(); it++) {
		delete *it;
	}
	_elements.clear();

	delete _cursorMan;
	delete _screen;
}

void BiikGUI::onMouseClick(bool isRight, const Common::Point &pos) {
	int icon = -1;
	for (ElementArray::const_iterator it = _elements.begin(); it != _elements.end(); it++) {
		if ((*it)->contains(pos)) {
			icon = (*it)->getID();
		}
	}

	if (icon == -1) {
		debugC(kDebugGUI, "User clicked the mouse: isRight: %d, pos: %d, %d", isRight, pos.x, pos.y);
		return;
	}

	if (icon == 0) {
		debugC(kDebugGUI, "User clicked on the stage: isRight: %d, pos: %d, %d", isRight, pos.x, pos.y);
		return;
	}

	debugC(kDebugGUI, "User clicked on icon %d: isRight: %d, pos: %d, %d", icon, isRight, pos.x, pos.y);
}

Common::Error BiikGUI::run() {
	while (!_quit) {
		Common::Event event;
		while (g_system->getEventManager()->pollEvent(event)) {
			if (processEvent(event))
				continue;

			switch (event.type) {
			case Common::EVENT_QUIT:
			case Common::EVENT_RETURN_TO_LAUNCHER:
				_quit = true;
				break;

			case Common::EVENT_LBUTTONDOWN:
				onMouseClick(false, event.mouse);
				break;

			case Common::EVENT_RBUTTONDOWN:
				onMouseClick(true, event.mouse);
				break;

			default:
				processEvent(event);
				break;
			}
		}

		updateScreen();
		g_system->delayMillis(10);
	}

	return Common::kNoError;
}

void BiikGUI::updateScreen() {
	_screen->update();
}

} // End of namespace Biik
