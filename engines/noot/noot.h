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

#ifndef NOOT_H
#define NOOT_H

#include "common/rect.h"
#include "engines/engine.h"
#include "graphics/palette.h"

struct ADGameDescription;

namespace Graphics {
class Font;
struct Surface;
}

namespace Image {
class ROSpriteArea;
}

namespace Noot {
class AnimationWidget;
class Book;
class Widget;

class NootEngine : public Engine {
private:
	const ADGameDescription *_gameDescription;

	Book *_book;

	Image::ROSpriteArea *_spriteArea;
	Graphics::PaletteLookup _palette;
	Common::Rect _screenRect;
	uint _xeig, _yeig;

	AnimationWidget *_animation;
	Common::Rect _animationRect;

	Common::Rect _textRect, _textRect1;
	Graphics::Font *_font;

	Widget *_nextButton;
	Common::Rect _nextRect;

	Widget *_input;
	Common::Rect _inputRect;

	bool _debugRects;

	void setScreenMode();

	Common::Error loadFont(const Common::String &name, int size);
	Common::Error loadSprites(const Common::Path &filename);

	void reflowText(uint maxChars);

	Common::Point convertMouse(const Common::Point &mouse) const;

protected:
	// Engine APIs
	Common::Error run() override;

public:
	NootEngine(OSystem *syst, const ADGameDescription *gameDesc);
	~NootEngine() override;

	bool hasFeature(EngineFeature f) const override {
		return (f == kSupportsReturnToLauncher || f == kSupportsChangingOptionsDuringRuntime);
	}

	void applyGameSettings() override;

	const Book *getBook() const {
		return _book;
	}

	const Image::ROSpriteArea *getSpriteArea() const {
		return _spriteArea;
	}

	const Graphics::Font *getFont() const {
		return _font;
	}

	uint getXEigFactor() const {
		return _xeig;
	}

	uint getYEigFactor() const {
		return _yeig;
	}

	bool getDebugRects() const {
		return _debugRects;
	}

	void setDebugRects(bool debugRects);

	bool loadAnimation(uint32 pos);

	Graphics::Surface *scaleSurface(const Graphics::Surface *surf, uint xeig, uint yeig) const;
	uint32 findBestColor(byte r, byte g, byte b);
	uint32 *createMap(const byte *srcPalette, uint len);
	void copyToScreen(const Graphics::Surface *surf, const Graphics::Surface *mask, const uint32 *map, const Common::Rect &dstRect);
	void drawRect(const Common::Rect &dstRect);
	void drawText(const Common::String &str, const Common::Rect &dstRect);
};

} // End of namespace Noot

#endif // NOOT_H
