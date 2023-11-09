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

#ifndef IMAGE_ROSPRITE_H
#define IMAGE_ROSPRITE_H

#include "common/scummsys.h"
#include "common/str.h"
#include "common/archive.h"
#include "image/image_decoder.h"

namespace Common {
class SeekableReadStream;
class WriteStream;
}

namespace Graphics {
struct Surface;
}

namespace Image {

/**
 * @defgroup image_sprite RISC OS Sprite decoder
 * @ingroup image
 *
 * @brief Decoder for RISC OS Sprite images.
 *
 * Used in engines:
 *  - Noot
 * @{
 */

class Codec;

class ROSpriteDecoder : public ImageDecoder {
public:
	ROSpriteDecoder();
	virtual ~ROSpriteDecoder();

	// ImageDecoder API
	void destroy();
	bool loadStream(Common::SeekableReadStream &stream) override;
	const Graphics::Surface *getSurface() const override { return _surface; }
	const byte *getPalette() const override { return _palette; }
	uint16 getPaletteColorCount() const override { return _paletteColorCount; }

	const Graphics::Surface *getMask() const { return _mask; }
	uint8 getXEigFactor() const { return _xeig; }
	uint8 getYEigFactor() const { return _yeig; }

private:
	Graphics::Surface *_surface, *_mask;
	byte *_palette;
	uint16 _paletteColorCount;
	uint8 _xeig, _yeig;

	uint8 dpiToEig(uint dpi) const {
		switch (dpi) {
		case 22:
		case 23: return 3;
		case 45: return 2;
		case 90: return 1;
		case 180: return 0;
		default: return 1;
		}
	}

	void readSprite(Common::SeekableReadStream &stream, Graphics::Surface &surf, uint offset, uint pitch);
	void convertSprite1(Common::SeekableReadStream &stream, Graphics::Surface &surf, uint offset, uint pitch);
	void convertSprite2(Common::SeekableReadStream &stream, Graphics::Surface &surf, uint offset, uint pitch);
	void convertSprite4(Common::SeekableReadStream &stream, Graphics::Surface &surf, uint offset, uint pitch);
#ifndef SCUMM_LITTLE_ENDIAN
	void convertSprite16(Common::SeekableReadStream &stream, Graphics::Surface &surf, uint offset, uint pitch);
#endif
};

class ROSpriteArea : public Common::Archive {
public:
	ROSpriteArea();
	virtual ~ROSpriteArea();

	bool open(const Common::Path &filename);
	void close();

	// Archive API
	bool hasFile(const Common::Path &path) const override;
	int listMembers(Common::ArchiveMemberList &list) const override;
	const Common::ArchiveMemberPtr getMember(const Common::Path &path) const override;
	Common::SeekableReadStream *createReadStreamForMember(const Common::Path &path) const override;

	ROSpriteDecoder *createDecoderForMember(const Common::Path &path) const;

private:
	struct SpriteEntry {
		uint32 offset;
		uint32 size;
	};

	Common::SeekableReadStream *_stream;

	typedef Common::HashMap<Common::Path, SpriteEntry, Common::Path::IgnoreCase_Hash, Common::Path::IgnoreCase_EqualTo> SpriteMap;
	SpriteMap _map;
};

extern const byte riscos_mode_vars[50][3];
extern const byte *const riscos_palettes[4];

} // End of namespace Image

#endif
