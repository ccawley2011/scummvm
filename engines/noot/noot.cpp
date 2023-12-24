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

#include "noot/noot.h"
#include "noot/console.h"

#include "common/config-manager.h"
#include "common/events.h"
#include "common/system.h"

#include "engines/util.h"

#include "graphics/blit.h"
#include "graphics/surface.h"

#include "image/rosprite.h"

namespace Noot {

NootEngine::NootEngine(OSystem *syst, const ADGameDescription *gameDesc) : Engine(syst),
	_gameDescription(gameDesc),
	// _screenRect(0, 0, 1276, 984),
	_screenRect(0, 0, 1280, 960),
	_nextRect(1202, 168, 1258, 252),
	_nextoff(nullptr),
	_nexton(nullptr),
	_nextoffMap(nullptr),
	_nextonMap(nullptr) {
}

NootEngine::~NootEngine() {
	delete _nextoff;
	delete _nexton;
	delete _nextoffMap;
	delete _nextonMap;
}

Common::Error NootEngine::run() {
	uint width = _screenRect.width() >> 1;
	uint height = _screenRect.height() >> 1;
	initGraphics(width, height);

	setDebugger(new Console());

	_palette.setPalette(Image::riscos_palettes[3], 256);
	g_system->getPaletteManager()->setPalette(Image::riscos_palettes[3], 0, 256);
	g_system->fillScreen(_palette.findBestColor(0xDD, 0xDD, 0xDD));

	Common::Error err = loadSprites("Sprites");
	if (err.getCode() != Common::kNoError)
		return err;

	if (_nextoff)
		copyToScreen(*_nextoff, _nextoffMap, _nextRect);

	g_system->updateScreen();

	// Simple event handling loop
	Common::Event e;
	while (!shouldQuit()) {
		while (g_system->getEventManager()->pollEvent(e)) {
		}
		// Delay for a bit. All events loops should have a delay
		// to prevent the system being unduly loaded
		g_system->delayMillis(10);
	}

	return Common::kNoError;
}

Common::Error NootEngine::loadSprites(const Common::Path &filename) {
	Image::ROSpriteArea spriteArea;
	if (!spriteArea.open(filename))
		return Common::Error(Common::kNoGameDataFoundError, filename.toString());

	_nextoff = spriteArea.createDecoderForMember("nextoff");
	if (_nextoff)
		_nextoffMap = _palette.createMap(_nextoff->getPalette(), _nextoff->getPaletteColorCount());

	_nexton = spriteArea.createDecoderForMember("nexton");
	if (_nexton)
		_nextonMap = _palette.createMap(_nexton->getPalette(), _nexton->getPaletteColorCount());

	return Common::kNoError;
}

void NootEngine::copyToScreen(const Image::ROSpriteDecoder &decoder, const uint32 *map, const Common::Rect &dstRect) {
	const Graphics::Surface *surf = decoder.getSurface();
	const Graphics::Surface *mask = decoder.getMask();
	uint xeig = decoder.getXEigFactor();
	uint yeig = decoder.getYEigFactor();

	Common::Rect rect(surf->w << xeig, surf->h << yeig);
	Common::Point pos(dstRect.left, dstRect.top);

	if (!Common::Rect::getBlitRect(pos, rect, _screenRect))
		return;

	pos.y = _screenRect.height() - pos.y;

	Graphics::Surface *screen = g_system->lockScreen();
	if (screen) {
		const byte *srcPtr = (const byte *)surf->getBasePtr(rect.left >> 1, rect.top >> 1);
		byte *dstPtr = (byte *)screen->getBasePtr(pos.x >> 1, pos.y >> 1);
		uint srcPitch = surf->pitch;
		uint dstPitch = screen->pitch;
		uint w = rect.width() >> 1, h = rect.height() >> 1;

		if (mask) {
			const byte *maskPtr = (const byte *)mask->getBasePtr(rect.left >> 1, rect.top >> 1);
			uint maskPitch = mask->pitch;

			if (map) {
				Graphics::crossMaskBlitMap(dstPtr, srcPtr, maskPtr, dstPitch, srcPitch, maskPitch,
					w, h, screen->format.bytesPerPixel, map);
			} else {
				Graphics::maskBlit(dstPtr, srcPtr, maskPtr, dstPitch, srcPitch, maskPitch,
					w, h, screen->format.bytesPerPixel);
			}
		} else {
			if (map) {
				Graphics::crossBlitMap(dstPtr, srcPtr, dstPitch, srcPitch,
					w, h, screen->format.bytesPerPixel, map);
			} else {
				Graphics::copyBlit(dstPtr, srcPtr, dstPitch, srcPitch,
					w, h, screen->format.bytesPerPixel);
			}
		}

		g_system->unlockScreen();
	}
}

} // End of namespace Noot
