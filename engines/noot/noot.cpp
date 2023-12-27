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
#include "noot/animation.h"
#include "noot/book.h"
#include "noot/console.h"

#include "common/config-manager.h"
#include "common/events.h"
#include "common/stream.h"
#include "common/system.h"

#include "engines/advancedDetector.h"
#include "engines/util.h"

#include "graphics/blit.h"
#include "graphics/surface.h"

#include "image/rosprite.h"

namespace Noot {

NootEngine::NootEngine(OSystem *syst, const ADGameDescription *gameDesc) : Engine(syst),
	_gameDescription(gameDesc),
	_book(nullptr),
	// _screenRect(0, 0, 1276, 984),
	_screenRect(0, 0, 1280, 960),
	_animation(nullptr),
	_animationMap(nullptr),
	_animationRect(172, 276, 1104, 936),
	_nextRect(1202, 84, 1258, 168),
	_nextoff(nullptr),
	_nexton(nullptr),
	_nextoffMap(nullptr),
	_nextonMap(nullptr),
	_debugRects(true),
	_xeig(1),
	_yeig(1) {
}

NootEngine::~NootEngine() {
	if (_nextoff) {
		_nextoff->free();
		delete _nextoff;
	}
	if (_nexton) {
		_nexton->free();
		delete _nexton;
	}

	if (_nextoffMask) {
		_nextoffMask->free();
		delete _nextoffMask;
	}

	if (_nextonMask) {
		_nextonMask->free();
		delete _nextonMask;
	}

	delete _animation;
	delete[] _animationMap;
	delete _book;
	delete[] _nextoffMap;
	delete[] _nextonMap;
}

Common::Error NootEngine::run() {
	uint width = _screenRect.width() >> _xeig;
	uint height = _screenRect.height() >> _yeig;
	initGraphics(width, height);

	setDebugger(new Console(this));

	Common::Path bookPath(_gameDescription->filesDescriptions[0].fileName);
	_book = new Book();
	if (!_book->open(bookPath))
		return Common::Error(Common::kNoGameDataFoundError, bookPath.toString());

	_palette.setPalette(Image::riscos_palettes[3], 256);
	g_system->getPaletteManager()->setPalette(Image::riscos_palettes[3], 0, 256);
	g_system->fillScreen(_palette.findBestColor(0xDD, 0xDD, 0xDD));

	Common::Error err = loadSprites("Sprites");
	if (err.getCode() != Common::kNoError)
		return err;

	if (_nextoff)
		copyToScreen(_nextoff, _nextoffMask, _nextoffMap, _nextRect);
	drawRect(_nextRect);

	loadAnimation(1);

	// Simple event handling loop
	Common::Event e;
	while (!shouldQuit()) {
		while (g_system->getEventManager()->pollEvent(e)) {
		}
		pollAnimation();
		g_system->updateScreen();

		// Delay for a bit. All events loops should have a delay
		// to prevent the system being unduly loaded
		g_system->delayMillis(10);
	}

	return Common::kNoError;
}

bool NootEngine::loadAnimation(uint32 pos) {
	Common::SeekableReadStream *stream = _book->loadResource(Book::kResourceAnimation, pos);
	if (!stream)
		return false;

	Animation *anim = new Animation();
	if (!anim->loadStream(stream)) {
		delete stream;
		return false;
	}

	delete _animation;
	_animation = anim;
	_animation->start();

	return true;
}

void NootEngine::pollAnimation() {
	if (!_animation || !_animation->needsUpdate())
		return;

	const Graphics::Surface *frame = _animation->decodeNextFrame();
	if (frame) {
		if (_animation->hasDirtyPalette()) {
			delete[] _animationMap;
			_animationMap = _palette.createMap(_animation->getPalette(), 256);
		}

		if (_xeig == _animation->getXEigFactor() && _yeig == _animation->getYEigFactor()) {
			copyToScreen(frame, nullptr, _animationMap, _animationRect);
		} else {
			Graphics::Surface *scaled = scaleSurface(frame, _animation->getXEigFactor(), _animation->getYEigFactor());
			copyToScreen(scaled, nullptr, _animationMap, _animationRect);
			scaled->free();
			delete scaled;
		}

		if (_debugRects) {
			Common::Rect dirtyRect(_animation->getDirtyRect());
			dirtyRect.translate(_animationRect.left, _animationRect.top);
			drawRect(_animationRect);
			drawRect(dirtyRect);
		}
	}
}

Common::Error NootEngine::loadSprites(const Common::Path &filename) {
	Image::ROSpriteDecoder *decoder;
	Image::ROSpriteArea spriteArea;

	if (!spriteArea.open(filename))
		return Common::Error(Common::kNoGameDataFoundError, filename.toString());

	decoder = spriteArea.createDecoderForMember("nextoff");
	if (decoder) {
		_nextoff = scaleSurface(decoder->getSurface(), decoder->getXEigFactor(), decoder->getYEigFactor());
		_nextoffMask = scaleSurface(decoder->getMask(), decoder->getXEigFactor(), decoder->getYEigFactor());
		_nextoffMap = _palette.createMap(decoder->getPalette(), decoder->getPaletteColorCount());
		delete decoder;
	}

	decoder = spriteArea.createDecoderForMember("nexton");
	if (decoder) {
		_nexton = scaleSurface(decoder->getSurface(), decoder->getXEigFactor(), decoder->getYEigFactor());
		_nextonMask = scaleSurface(decoder->getMask(), decoder->getXEigFactor(), decoder->getYEigFactor());
		_nextonMap = _palette.createMap(decoder->getPalette(), decoder->getPaletteColorCount());
		delete decoder;
	}

	return Common::kNoError;
}

Graphics::Surface *NootEngine::scaleSurface(const Graphics::Surface *surf, uint xeig, uint yeig) const {
	if (!surf)
		return nullptr;

	uint newWidth = (surf->w << xeig) >> _xeig;
	uint newHeight = (surf->h << yeig) >> _yeig;
	return surf->scale(newWidth, newHeight);
}

void NootEngine::copyToScreen(const Graphics::Surface *surf, const Graphics::Surface *mask, const uint32 *map, const Common::Rect &dstRect) {
	Common::Rect rect(surf->w << _xeig, surf->h << _yeig);
	Common::Point pos(dstRect.left, dstRect.bottom);

	if (!Common::Rect::getBlitRect(pos, rect, _screenRect))
		return;

	pos.y = _screenRect.height() - pos.y;

	Graphics::Surface *screen = g_system->lockScreen();
	if (screen) {
		const byte *srcPtr = (const byte *)surf->getBasePtr(rect.left >> _xeig, rect.top >> _yeig);
		byte *dstPtr = (byte *)screen->getBasePtr(pos.x >> _xeig, pos.y >> _yeig);
		uint srcPitch = surf->pitch;
		uint dstPitch = screen->pitch;
		uint w = surf->w, h = surf->h;

		if (mask) {
			const byte *maskPtr = (const byte *)mask->getBasePtr(rect.left >> _xeig, rect.top >> _yeig);
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

void NootEngine::drawRect(const Common::Rect &dstRect) {
	uint left = dstRect.left >> _xeig;
	uint right = dstRect.right >> _xeig;
	uint top = (_screenRect.height() - dstRect.bottom) >> _yeig;
	uint bottom = (_screenRect.height() - dstRect.top) >> _yeig;
	Common::Rect rect(left, top, right, bottom);

	Graphics::Surface *screen = g_system->lockScreen();
	if (screen) {
		screen->frameRect(rect, 0);
		g_system->unlockScreen();
	}
}

} // End of namespace Noot
