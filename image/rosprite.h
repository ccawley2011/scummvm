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

/**
 * @file
 * Image decoder used in engines:
 *  - biik
 */

#ifndef IMAGE_ROSPRITE_H
#define IMAGE_ROSPRITE_H

#include "common/scummsys.h"
#include "common/hash-str.h"
#include "common/str.h"
#include "image/image_decoder.h"

namespace Common {
class SeekableReadStream;
}

namespace Graphics {
struct Surface;
}

namespace Image {

class ROSpriteDecoder : public ImageDecoder {
public:
	ROSpriteDecoder();
	virtual ~ROSpriteDecoder();

	// ImageDecoder API
	void destroy();
	virtual bool loadStream(Common::SeekableReadStream &stream);
	virtual const Graphics::Surface *getSurface() const { return _surface; }
	const byte *getPalette() const { return _palette; }
	uint16 getPaletteColorCount() const { return _paletteColorCount; }

	const Graphics::Surface *getMask() const { return _mask; }
	uint8 getXEigFactor() const { return _xeig; }
	uint8 getYEigFactor() const { return _yeig; }

private:
	bool readSpriteType(uint32 spriteType, uint8 &log2BPP, uint8 &xeig, uint8 &yeig);

	void convertMask(Common::SeekableReadStream &stream, const int div, const int mask, const int shift);
	void convertSprite124(Common::SeekableReadStream &stream, const int div, const int mask, const int shift);
	void convertSprite8(Common::SeekableReadStream &stream);

	Graphics::Surface *_surface, *_mask;
	byte *_palette;
	uint16 _paletteColorCount;
	uint8 _xeig, _yeig;

	uint32 _width, _height;
	uint32 _spriteOffset, _maskOffset;
};

class ROSpriteArea {
public:
	virtual ~ROSpriteArea() { destroy(); }

	void destroy() { _map.clear(); }
	bool loadStream(Common::SeekableReadStream &stream, bool isArea = false);
	ROSpriteDecoder *getSprite(Common::String name) { return _map[name]; }
	ROSpriteDecoder *getSprite(int count) { return _map[_names[count]]; }

private:
	typedef Common::HashMap<Common::String, ROSpriteDecoder *, Common::IgnoreCase_Hash, Common::IgnoreCase_EqualTo> SpriteMap;
	SpriteMap _map;

	Common::Array<Common::String> _names;

	uint32 _headerSize, _count;
};

extern const int riscos_mode_vars[50][3];

extern const byte riscos_palette_2[2 * 3];
extern const byte riscos_palette_4[4 * 3];
extern const byte riscos_palette_16[16 * 3];
extern const byte riscos_palette_256[256 * 3];

extern const byte riscos_palette_16_to_256[16];

} // End of namespace Image

#endif
