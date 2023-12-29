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
#include "noot/book.h"
#include "noot/console.h"
#include "noot/dialogs.h"
#include "noot/widgets.h"

#include "common/config-manager.h"
#include "common/events.h"
#include "common/stream.h"
#include "common/system.h"

#include "engines/advancedDetector.h"
#include "engines/util.h"

#include "graphics/blit.h"
#include "graphics/cursorman.h"
#include "graphics/font.h"
#include "graphics/surface.h"
#include "graphics/fonts/ttf.h"

#include "image/rosprite.h"

namespace Noot {

static const byte MOUSECURSOR_RISCOS_HI[] = {
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

static const byte MOUSECURSOR_RISCOS_LO[] = {
	1,1,0,0,0,0,0,0,0,0,0,
	1,2,1,1,0,0,0,0,0,0,0,
	1,2,2,2,1,1,0,0,0,0,0,
	1,2,2,2,2,2,1,1,1,0,0,
	1,2,2,2,2,2,2,2,2,1,1,
	1,2,2,2,2,2,1,1,1,1,1,
	1,2,1,1,1,2,2,1,0,0,0,
	1,1,1,0,0,1,2,2,1,0,0,
	0,0,0,0,0,0,1,2,2,1,0,
	0,0,0,0,0,0,0,1,2,2,1,
	0,0,0,0,0,0,0,0,1,1,1,
};

static const byte cursorPalette[] = {
	0x00, 0x00, 0x00,  // Black / Transparent
	0x00, 0xff, 0xff,  // Cyan
	0x00, 0x00, 0x99,  // Blue
	0xff, 0x00, 0x00   // Red
};

NootEngine::NootEngine(OSystem *syst, const ADGameDescription *gameDesc) : Engine(syst),
	_gameDescription(gameDesc),
	_book(nullptr),
	// _screenRect(0, 0, 1276, 984),
	_screenRect(0, 0, 1280, 960),
	_animation(nullptr),
	_animationRect(172, 276, 1104, 936),
	_text(nullptr),
	_textRect(8, 18, 1262, 244),
	_textRect1(8, 18, 1202, 244),
	_font(nullptr),
	_nextButton(nullptr),
	_nextRect(1202, 84, 1258, 168),
	_input(nullptr),
	_debugRects(false),
	_xeig(1),
	_yeig(1) {
}

NootEngine::~NootEngine() {
	delete _input;
	delete _nextButton;
	delete _text;
	delete _animation;
	delete _book;
	delete _font;
	delete _spriteArea;
}

Common::Error NootEngine::run() {
	setDebugger(new Console(this));

	Common::Path bookPath(_gameDescription->filesDescriptions[0].fileName);
	_book = new Book();
	if (!_book->open(bookPath))
		return Common::Error(Common::kNoGameDataFoundError, bookPath.toString());

	setScreenMode();

	Common::Error err = loadSprites("Sprites");
	if (err.getCode() != Common::kNoError)
		return err;

	err = loadFont("LiberationSerif-Regular.ttf", 20);
	if (err.getCode() != Common::kNoError)
		return err;

	reflowText(3);

	_animation = new AnimationWidget(this, _animationRect);
	_nextButton = new ButtonWidget(this, _nextRect, "nextoff", "nexton");
	_input = new InputWidget(this, _inputRect, 3);
	_text = new TextWidget(this, _textRect1, "This is an example string");

	drawRect(_textRect, _fgColour);

	loadAnimation(1);

	// Simple event handling loop
	Common::Event e;
	while (!shouldQuit()) {
		while (g_system->getEventManager()->pollEvent(e)) {
			switch (e.type) {
			case Common::EVENT_MOUSEMOVE:
				_nextButton->handleMouseMotion(convertMouse(e.mouse));
				break;
			case Common::EVENT_KEYDOWN:
				if (_input)
					_input->handleKeyDown(e.kbd);
				break;
			case Common::EVENT_KEYUP:
				if (_input)
					_input->handleKeyUp(e.kbd);
				break;
			default:
				break;
			}
		}
		if (_animation->isDirty())
			_animation->render();
		if (_text->isDirty())
			_text->render();
		if (_nextButton->isDirty())
			_nextButton->render();
		if (_input->isDirty())
			_input->render();
		g_system->updateScreen();

		// Delay for a bit. All events loops should have a delay
		// to prevent the system being unduly loaded
		g_system->delayMillis(10);
	}

	return Common::kNoError;
}

void NootEngine::applyGameSettings() {
	bool modeChanged = false;
	uint mode = ConfMan.getInt("resolution");
	uint xeig = NootOptionsWidget::resolutions[mode].xeig;
	uint yeig = NootOptionsWidget::resolutions[mode].yeig;

	if (xeig != _xeig || yeig != _yeig) {
		setScreenMode();
		modeChanged = true;
	}

	if (modeChanged) {
		loadFont("LiberationSerif-Regular.ttf", 20);

		fillRect(_textRect, _bgColour);
		drawRect(_textRect, _fgColour);

		if (_animation)
			_animation->load();
		if (_text)
			_text->load();
		if (_nextButton)
			_nextButton->load();
		if (_input)
			_input->load();
	}
}

void NootEngine::setScreenMode() {
	Graphics::PixelFormat format = Graphics::PixelFormat::createFormatCLUT8();
	Graphics::ModeWithFormatList modes = {
		Graphics::ModeWithFormat(_screenRect.width() >> 0, _screenRect.height() >> 0, format),
		Graphics::ModeWithFormat(_screenRect.width() >> 1, _screenRect.height() >> 1, format),
		Graphics::ModeWithFormat(_screenRect.width() >> 1, _screenRect.height() >> 2, format),
		Graphics::ModeWithFormat(_screenRect.width() >> 2, _screenRect.height() >> 2, format),
	};
	int mode = initGraphicsAny(modes, ConfMan.getInt("resolution"));
	_xeig = NootOptionsWidget::resolutions[mode].xeig;
	_yeig = NootOptionsWidget::resolutions[mode].yeig;


	const Book::BookHeader *header = _book->getHeader();

	uint32 log2bpp = Image::riscos_mode_vars[header->mode][2];
	_palette.setPalette(Image::riscos_palettes[log2bpp], 1 << (1 << log2bpp));
	g_system->getPaletteManager()->setPalette(Image::riscos_palettes[log2bpp], 0, 1 << (1 << log2bpp));

	_bgColour = findBestColor(0xDD, 0xDD, 0xDD);
	_fgColour = findBestColor(0,    0,    0);
	g_system->fillScreen(_bgColour);

	loadCursor();
}

void NootEngine::setDebugRects(bool debugRects) {
	_debugRects = debugRects;

	if (_animation)
		_animation->invalidate();
	if (_text)
		_text->invalidate();
	if (_nextButton)
		_nextButton->invalidate();
	if (_input)
		_input->invalidate();
}

bool NootEngine::loadAnimation(uint32 pos) {
	Common::SeekableReadStream *stream = _book->loadResource(Book::kResourceAnimation, pos);
	if (!stream)
		return false;

	if (!_animation->loadStream(stream)) {
		delete stream;
		return false;
	}

	return true;
}

void NootEngine::loadCursor() {
	if (_xeig == 1 && _yeig == 1) {
		CursorMan.replaceCursor(MOUSECURSOR_RISCOS_HI, 12, 22, 0, 0, 0);
	} else if (_xeig == 1 && _yeig == 2) {
		CursorMan.replaceCursor(MOUSECURSOR_RISCOS_LO, 11, 11, 0, 0, 0);
	} else {
		// TODO: Get the backend to handle scaling

		Graphics::Surface cursor;
		Graphics::Surface *scaledCursor;

		if (_yeig > 1) {
			cursor.init(11, 11, 11, const_cast<byte *>(MOUSECURSOR_RISCOS_LO),
			            Graphics::PixelFormat::createFormatCLUT8());
			scaledCursor = scaleSurface(&cursor, 1, 2);
		} else {
			cursor.init(12, 22, 12, const_cast<byte *>(MOUSECURSOR_RISCOS_HI),
			            Graphics::PixelFormat::createFormatCLUT8());
			scaledCursor = scaleSurface(&cursor, 1, 1);
		}

		CursorMan.replaceCursor(*scaledCursor, 0, 0, 0);
		scaledCursor->free();
		delete scaledCursor;
	}

	CursorMan.replaceCursorPalette(cursorPalette, 0, 4);
	CursorMan.showMouse(true);
}

Common::Error NootEngine::loadFont(const Common::String &name, int size) {
	if (_font)
		delete _font;
	_font = loadTTFFontFromArchive(name, size, Graphics::kTTFSizeModeCharacter, 180 >> _xeig, 180 >> _yeig, Graphics::kTTFRenderModeMonochrome);
	if (_font)
		return Common::kNoError;

	return Common::Error(Common::kNoGameDataFoundError, name);
}

Common::Error NootEngine::loadSprites(const Common::Path &filename) {
	_spriteArea = new Image::ROSpriteArea();

	if (!_spriteArea->open(filename))
		return Common::Error(Common::kNoGameDataFoundError, filename.toString());

	return Common::kNoError;
}

void NootEngine::reflowText(uint maxChars) {
	uint w = _font->getMaxCharWidth() << _xeig;
	uint h = _font->getFontHeight() << _yeig;

	_inputRect = Common::Rect(0, 0, w * maxChars, h + 12);

	if (_input) {
		delete _input;
		_input = new InputWidget(this, _inputRect, maxChars);
	}
}

Graphics::Surface *NootEngine::scaleSurface(const Graphics::Surface *surf, uint xeig, uint yeig) const {
	if (!surf)
		return nullptr;

	uint newWidth = (surf->w << xeig) >> _xeig;
	uint newHeight = (surf->h << yeig) >> _yeig;
	return surf->scale(newWidth, newHeight);
}

uint32 NootEngine::findBestColor(byte r, byte g, byte b) {
	return _palette.findBestColor(r, g, b);
}

uint32 *NootEngine::createMap(const byte *srcPalette, uint len) {
	return _palette.createMap(srcPalette, len);
}

Common::Point NootEngine::convertMouse(const Common::Point &mouse) const {
	return Common::Point(mouse.x << _xeig, _screenRect.height() - (mouse.y << _yeig));
}

Graphics::Surface *NootEngine::lockScreen(const Common::Rect &dstRect) {
	uint left = dstRect.left >> _xeig;
	uint right = dstRect.right >> _xeig;
	uint top = (_screenRect.height() - dstRect.bottom) >> _yeig;
	uint bottom = (_screenRect.height() - dstRect.top) >> _yeig;
	Common::Rect rect(left, top, right, bottom);

	Graphics::Surface *screen = g_system->lockScreen();
	if (!screen)
		return nullptr;


	Graphics::Surface *subArea = new Graphics::Surface();
	subArea->init(rect.width(), rect.height(), screen->pitch,
	              screen->getBasePtr(rect.left, rect.top), screen->format);
	return subArea;
}

void NootEngine::unlockScreen(Graphics::Surface *screen) {
	g_system->unlockScreen();
	delete screen;
}

void NootEngine::copyRectToScreen(const Graphics::Surface *surf, const Common::Rect &dstRect) {
	if (!surf)
		return;

	uint left = dstRect.left >> _xeig;
	uint right = dstRect.right >> _xeig;
	uint top = (_screenRect.height() - dstRect.bottom) >> _yeig;
	uint bottom = (_screenRect.height() - dstRect.top) >> _yeig;
	Common::Rect rect(left, top, right, bottom);

	if (surf->w == rect.width() && surf->h == rect.height()) {
		g_system->copyRectToScreen(surf->getPixels(), surf->pitch,
		                           rect.left, rect.top, rect.width(), rect.height());
	} else {
		Graphics::Surface *screen = lockScreen(dstRect);
		if (screen) {
			const byte *srcPtr = (const byte *)surf->getPixels();
			byte *dstPtr = (byte *)screen->getPixels();

			Graphics::scaleBlit(dstPtr, srcPtr, screen->pitch, surf->pitch,
				screen->w, screen->h, surf->w, surf->h, screen->format);

			unlockScreen(screen);
		}
	}
}

void NootEngine::copyRectToScreen(const Graphics::Surface *surf, const Common::Rect &dstRect, const uint32 *map, const Graphics::Surface *mask) {
	if (!surf)
		return;

	if (!map && !mask) {
		copyRectToScreen(surf, dstRect);
		return;
	}

	uint left = dstRect.left >> _xeig;
	uint right = dstRect.right >> _xeig;
	uint top = (_screenRect.height() - dstRect.bottom) >> _yeig;
	uint bottom = (_screenRect.height() - dstRect.top) >> _yeig;
	Common::Rect rect(left, top, right, bottom);

	Graphics::Surface *scaledSurf = nullptr, *scaledMask = nullptr;;
	if (surf->w != rect.width() || surf->h != rect.height()) {
		surf = scaledSurf = surf->scale(rect.width(), rect.height());
		if (mask)
			mask = scaledMask = surf->scale(rect.width(), rect.height());
	}

	Graphics::Surface *screen = lockScreen(dstRect);
	if (screen) {
		const byte *srcPtr = (const byte *)surf->getPixels();
		byte *dstPtr = (byte *)screen->getPixels();
		uint srcPitch = surf->pitch;
		uint dstPitch = screen->pitch;
		uint w = surf->w, h = surf->h;

		if (mask) {
			const byte *maskPtr = (const byte *)mask->getPixels();
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

		unlockScreen(screen);
	}

	if (scaledSurf) {
		scaledSurf->free();
		delete scaledSurf;
	}

	if (scaledMask) {
		scaledMask->free();
		delete scaledMask;
	}
}

void NootEngine::fillRect(const Common::Rect &dstRect, uint32 colour) {
	uint left = dstRect.left >> _xeig;
	uint right = dstRect.right >> _xeig;
	uint top = (_screenRect.height() - dstRect.bottom) >> _yeig;
	uint bottom = (_screenRect.height() - dstRect.top) >> _yeig;
	Common::Rect rect(left, top, right, bottom);

	g_system->fillScreen(rect, colour);
}

void NootEngine::drawRect(const Common::Rect &dstRect, uint32 colour) {
	Graphics::Surface *screen = lockScreen(dstRect);
	if (screen) {
		screen->frameRect(Common::Rect(screen->w, screen->h), colour);
		unlockScreen(screen);
	}
}

} // End of namespace Noot
