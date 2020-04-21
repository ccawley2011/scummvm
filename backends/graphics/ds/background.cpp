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

#include "backends/graphics/ds/background.h"

namespace DS {

Background::Background(int layer, BgType type, BgSize size, int mapBase, uint16 _maxWidth, uint16 _maxHeight)
	: dirtyPixels(true),
	  maxWidth(_maxWidth),
	  maxHeight(_maxHeight),
	  actualWidth(0),
	  actualHeight(0),
	  bg(0) {
	assert(type >= BgType_Bmp8);
	bg = bgInit(layer, type, size, mapBase, 0);
}

void Background::create(uint16 width, uint16 height, const Graphics::PixelFormat &f) {
	Graphics::Surface::create(maxWidth, maxHeight, f);

	actualWidth = width;
	actualHeight = height;
	dirtyPixels = true;
}

void Background::free() {
	Graphics::Surface::free();
	actualWidth = actualHeight = 0;
}

void Background::transfer() {
	if (pixels && dirtyPixels) {
		dirtyPixels = false;
		u16 *dst = bgGetGfxPtr(bg);
		dmaCopy(pixels, dst, w * h * format.bytesPerPixel);
	}
}

void Background::show() {
	bgShow(bg);
}

void Background::hide() {
	bgHide(bg);
}

void Background::convertToInPlace(const Graphics::PixelFormat &dstFormat, const byte *palette) {
	//
}

void Background::grab(void *buf, int dstPitch) {
	byte *dst = (byte *)buf;

	for (int y = 0; y < actualHeight; ++y) {
		memcpy(dst, getBasePtr(0, y), actualWidth * format.bytesPerPixel);
		dst += dstPitch;
	}
}

void Background::clear() {
	dirtyPixels = true;
	memset(pixels, 0, w * h * format.bytesPerPixel);
}

} // namespace DS
