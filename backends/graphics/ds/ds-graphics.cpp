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

#include <nds.h>

#include "backends/graphics/ds/ds-graphics.h"
#include "backends/graphics/ds/background.h"

namespace DS {

DSGraphicsManager::DSGraphicsManager() {
	videoSetMode(MODE_5_2D | DISPLAY_BG3_ACTIVE);
	vramSetPrimaryBanks(VRAM_A_MAIN_BG,VRAM_B_MAIN_BG,VRAM_C_MAIN_BG,VRAM_D_MAIN_BG);

	_overlay = new Background(3, BgType_Bmp16, BgSize_B16_256x256, 0, 256, 256);
	_overlay->create(256, 192, Graphics::PixelFormat(2, 5, 5, 5, 1, 0, 5, 10, 15));
}

DSGraphicsManager::~DSGraphicsManager() {
	_overlay->free();
	delete _overlay;
	_overlay = 0;
}

void DSGraphicsManager::updateScreen() {
	_overlay->transfer();
}

void DSGraphicsManager::showOverlay() {
	_overlay->show();
}

void DSGraphicsManager::hideOverlay() {
	_overlay->hide();
}

Graphics::PixelFormat DSGraphicsManager::getOverlayFormat() const {
	return _overlay->format;
}

void DSGraphicsManager::clearOverlay() {
	return _overlay->clear();
}

void DSGraphicsManager::grabOverlay(void *buf, int pitch) const {
	return _overlay->grab(buf, pitch);
}

void DSGraphicsManager::copyRectToOverlay(const void *buf, int pitch, int x, int y, int w, int h) {
	_overlay->copyRectToSurface(buf, pitch, x, y, w, h);
	_overlay->markDirty();
}

int16 DSGraphicsManager::getOverlayHeight() const {
	return _overlay->actualHeight;
}

int16 DSGraphicsManager::getOverlayWidth() const {
	return _overlay->actualWidth;
}

} // End of namespace DS
