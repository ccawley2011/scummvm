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
struct Surface;
}

namespace Image {
class ROSpriteDecoder;
}

namespace Noot {
class Animation;
class Book;

class NootEngine : public Engine {
private:
	const ADGameDescription *_gameDescription;

	Book *_book;

	Graphics::PaletteLookup _palette;
	Common::Rect _screenRect;

	Animation *_animation;
	uint32 *_animationMap;
	Common::Rect _animationRect;

	Image::ROSpriteDecoder *_nextoff, *_nexton;
	uint32 *_nextoffMap, *_nextonMap;
	Common::Rect _nextRect;

	bool _debugRects;

	void pollAnimation();

	Common::Error loadSprites(const Common::Path &filename);

	void copyToScreen(const Image::ROSpriteDecoder &decoder, const uint32 *map, const Common::Rect &dstRect);
	void copyToScreen(const Graphics::Surface *surf, const Graphics::Surface *mask, const uint32 *map, const Common::Rect &dstRect, uint xeig, uint yeig);
	void drawRect(const Common::Rect &dstRect);

protected:
	// Engine APIs
	Common::Error run() override;

public:
	NootEngine(OSystem *syst, const ADGameDescription *gameDesc);
	~NootEngine() override;

	bool hasFeature(EngineFeature f) const override {
		return (f == kSupportsReturnToLauncher);
	}

	const Book *getBook() const {
		return _book;
	}

	bool loadAnimation(uint32 pos);
};

} // End of namespace Noot

#endif // NOOT_H
