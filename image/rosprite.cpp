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

#include "image/rosprite.h"

#include "common/stream.h"
#include "common/substream.h"
#include "common/textconsole.h"
#include "graphics/pixelformat.h"
#include "graphics/surface.h"

namespace Image {

ROSpriteDecoder::ROSpriteDecoder() {
	_surface = 0;
	_mask = 0;
	_palette = 0;
	_paletteColorCount = 0;
	_xeig = 0;
	_yeig = 0;
}

ROSpriteDecoder::~ROSpriteDecoder() {
	destroy();
}

void ROSpriteDecoder::destroy() {
	if (_surface) {
		delete _surface;
		_surface = 0;
	}

	if (_palette) {
		delete[] _palette;
		_palette = 0;
	}

	_paletteColorCount = 0;
	_xeig = 0;
	_yeig = 0;
}

bool ROSpriteDecoder::loadStream(Common::SeekableReadStream &stream) {
	destroy();

	char name[12 + 1];

	/* uint32 size = */ stream.readUint32LE();
	stream.read(name, 12);
	name[12] = 0;
	uint32 width = stream.readUint32LE();
	uint32 height = stream.readUint32LE();
	uint32 firstBit = stream.readUint32LE();
	uint32 lastBit = stream.readUint32LE();
	uint32 spriteOffset = stream.readUint32LE();
	uint32 maskOffset = stream.readUint32LE();
	uint32 mode = stream.readUint32LE();

	Graphics::PixelFormat format;
	uint spriteType, log2BPP, modeFlags, aBits;

	if (mode > 0xff) {
		if ((mode & 0x780F000F) == 0x78000001) {
			// RISC OS 5 sprite mode word
			_xeig = (mode >> 4) & 0x03;
			_yeig = (mode >> 6) & 0x03;
			spriteType = (mode >> 20) & 0x7f;
			modeFlags = mode & 0xff00;
		} else {
			// RISC OS 3.5 sprite mode word
			_xeig = dpiToEig((mode >> 1)  & 0x1ff);
			_yeig = dpiToEig((mode >> 14) & 0x1ff);
			spriteType = (mode >> 27) & 0x0f;
			modeFlags = 0;
		}
	} else {
		// Numbered mode
		if (mode > ARRAYSIZE(riscos_mode_vars)) {
			warning("Unrecognised screen mode '%d'", mode);
			return false;
		}

		_xeig = riscos_mode_vars[mode][0];
		_yeig = riscos_mode_vars[mode][1];
		spriteType = riscos_mode_vars[mode][2] + 1;
		modeFlags = 0;
	}

	switch (spriteType) {
	case 1:
	case 2:
	case 3:
	case 4:
		format = Graphics::PixelFormat::createFormatCLUT8();
		log2BPP = spriteType - 1;
		_paletteColorCount = 1 << (1 << log2BPP);

		// Read the palette
		_palette = new byte[_paletteColorCount * 3];
		if (spriteOffset >= (uint)(44 + (_paletteColorCount * 8))) {
			byte entry[8];

			for (uint16 i = 0; i < _paletteColorCount; i++) {
				stream.read(entry, 8);
				_palette[i * 3 + 0] = entry[1];
				_palette[i * 3 + 1] = entry[2];
				_palette[i * 3 + 2] = entry[3];
			}
		} else {
			memcpy(_palette, riscos_palettes[log2BPP], _paletteColorCount * 3);
		}
		break;
	case 5:
		aBits = (modeFlags & 0x8000) ? 1 : 0;
		if (modeFlags & 0x4000)
			format = Graphics::PixelFormat(2, 5, 5, 5, aBits, 10, 5, 0, 15);
		else
			format = Graphics::PixelFormat(2, 5, 5, 5, aBits, 0, 5, 10, 15);
		log2BPP = 4;
		break;
	case 6:
		aBits = (modeFlags & 0x8000) ? 8 : 0;
#ifdef SCUMM_LITTLE_ENDIAN
		if (modeFlags & 0x4000)
			format = Graphics::PixelFormat(4, 8, 8, 8, aBits, 16, 8, 0, 24);
		else
			format = Graphics::PixelFormat(4, 8, 8, 8, aBits, 0, 8, 16, 24);
#else
		if (modeFlags & 0x4000)
			format = Graphics::PixelFormat(4, 8, 8, 8, aBits, 8, 16, 24, 0);
		else
			format = Graphics::PixelFormat(4, 8, 8, 8, aBits, 24, 16, 8, 0);
#endif
		log2BPP = 5;
		break;
	case 10:
		if (modeFlags & 0x4000)
			format = Graphics::PixelFormat(2, 5, 6, 5, 0, 11, 5, 0, 0);
		else
			format = Graphics::PixelFormat(2, 5, 6, 5, 0, 0, 5, 11, 0);
		log2BPP = 4;
		break;
	case 16:
		aBits = (modeFlags & 0x8000) ? 4 : 0;
		if (modeFlags & 0x4000)
			format = Graphics::PixelFormat(2, 4, 4, 4, aBits, 8, 4, 0, 12);
		else
			format = Graphics::PixelFormat(2, 4, 4, 4, aBits, 0, 4, 8, 12);
		log2BPP = 4;
		break;
	default:
		warning("Unsupported sprite type '%d' in sprite '%s'", spriteType, name);
		return false;
	}


	uint32 realWidth = (width * 32 + lastBit - firstBit + 1 ) >> log2BPP;
	uint32 realHeight = height + 1;
	uint32 realPitch = (width * 4) + 4;

	_surface = new Graphics::Surface();
	_surface->create(realWidth, realHeight, format);

	switch (spriteType) {
	case 1:
		convertSprite1(stream, *_surface, spriteOffset, realPitch);
		break;
	case 2:
		convertSprite2(stream, *_surface, spriteOffset, realPitch);
		break;
	case 3:
		convertSprite4(stream, *_surface, spriteOffset, realPitch);
		break;
	case 4:
	case 6:
		readSprite(stream, *_surface, spriteOffset, realPitch);
		break;
	case 5:
	case 10:
	case 16:
#ifdef SCUMM_LITTLE_ENDIAN
		readSprite(stream, *_surface, spriteOffset, realPitch);
#else
		convertSprite16(stream, *_surface, spriteOffset, realPitch);
#endif
		break;
	default:
		warning("Unsupported sprite type '%d' in sprite '%s'", spriteType, name);
		return false;
	}

	if (maskOffset != spriteOffset) {
		uint maskType = (mode > 0xff) ? 1 : spriteType;
		uint maskPitch = (mode > 0xff) ? ((width + 31) >> 5) : realPitch;

		_mask = new Graphics::Surface();
		_mask->create(realWidth, realHeight, Graphics::PixelFormat::createFormatCLUT8());

		switch (maskType) {
		case 1:
			convertSprite1(stream, *_mask, maskOffset, maskPitch);
			break;
		case 2:
			convertSprite2(stream, *_mask, maskOffset, maskPitch);
			break;
		case 3:
			convertSprite4(stream, *_mask, maskOffset, maskPitch);
			break;
		case 4:
			readSprite(stream, *_mask, maskOffset, maskPitch);
			break;
		default:
			warning("Unsupported mask type '%d' in sprite '%s'", maskType, name);
			delete _mask;
			_mask = nullptr;
			break;
		}
	}

	return true;
}

void ROSpriteDecoder::readSprite(Common::SeekableReadStream &stream, Graphics::Surface &surf, uint offset, uint pitch) {
	byte *dst = (byte *)surf.getPixels();

	for (int y = 0; y < surf.h; y++) {
		stream.seek(offset);
		stream.read(dst, surf.w * surf.format.bytesPerPixel);
		offset += pitch;
		dst += surf.pitch;
	}
}

void ROSpriteDecoder::convertSprite1(Common::SeekableReadStream &stream, Graphics::Surface &surf, uint offset, uint pitch) {
	byte *dst = (byte *)surf.getPixels();

	for (int y = 0; y < surf.h; y++) {
		stream.seek(offset);

		for (int x = 0; x < surf.w;) {
			byte color = stream.readByte();

			for (int i = 0; i < 8; i++) {
				dst[x++] = color & 0x01;
				color >>= 1;
				if (x == surf.w)
					break;
			}
		}

		offset += pitch;
		dst += surf.pitch;
	}
}

void ROSpriteDecoder::convertSprite2(Common::SeekableReadStream &stream, Graphics::Surface &surf, uint offset, uint pitch) {
	byte *dst = (byte *)surf.getPixels();

	for (int y = 0; y < surf.h; y++) {
		stream.seek(offset);

		for (int x = 0; x < surf.w;) {
			byte color = stream.readByte();

			for (int i = 0; i < 4; i++) {
				dst[x++] = color & 0x03;
				color >>= 2;
				if (x == surf.w)
					break;
			}
		}

		offset += pitch;
		dst += surf.pitch;
	}
}

void ROSpriteDecoder::convertSprite4(Common::SeekableReadStream &stream, Graphics::Surface &surf, uint offset, uint pitch) {
	byte *dst = (byte *)surf.getPixels();

	for (int y = 0; y < surf.h; y++) {
		stream.seek(offset);

		for (int x = 0; x < surf.w; x++) {
			byte color = stream.readByte();

			dst[x++] = color & 0x0f;

			if (x == surf.w)
				break;

			dst[x] = (color & 0xf0) >> 4;
		}

		offset += pitch;
		dst += surf.pitch;
	}
}

#ifndef SCUMM_LITTLE_ENDIAN

void ROSpriteDecoder::convertSprite16(Common::SeekableReadStream &stream, Graphics::Surface &surf, uint offset, uint pitch) {
	uint8 *dst = (uint8 *)surf.getPixels();

	for (int y = 0; y < surf.h; y++) {
		stream.seek(offset);
		stream.read(dst, surf.w * surf.format.bytesPerPixel);

		uint16 *row = (uint16 *)dst;
		for (int x = 0; x < surf.w; x++) {
			row[x] = FROM_LE_16(row[x]);
		}

		offset += pitch;
		dst += surf.pitch;
	}
}

#endif



ROSpriteArea::ROSpriteArea() : Common::Archive() {
	_stream = nullptr;
}

ROSpriteArea::~ROSpriteArea() {
	close();
}

bool ROSpriteArea::open(const Common::Path &filename) {
	close();

	_stream = SearchMan.createReadStreamForMember(filename);

	if (!_stream)
		return false;

	uint32 count = _stream->readUint32LE();
	uint32 first = _stream->readUint32LE() - 4;;
	uint32 last = _stream->readUint32LE() - 4;
	uint32 next = first;

	for (uint32 i = 0; i < count; i++) {
		SpriteEntry entry;
		char name[12 + 1];

		// Read enough of the sprite header to get the name and size.
		_stream->seek(next);
		entry.offset = next;
		entry.size = _stream->readUint32LE();
		_stream->read(name, 12);
		name[12] = 0;

		_map[name] = entry;

		next += entry.size;
		if (next >= last)
			break;
	}

	return true;
}

void ROSpriteArea::close() {
	delete _stream; _stream = nullptr;
	_map.clear();
}

bool ROSpriteArea::hasFile(const Common::Path &path) const {
	return _map.contains(path);
}

int ROSpriteArea::listMembers(Common::ArchiveMemberList &list) const {
	for (SpriteMap::const_iterator it = _map.begin(); it != _map.end(); it++)
		list.push_back(getMember(it->_key));

	return _map.size();
}

const Common::ArchiveMemberPtr ROSpriteArea::getMember(const Common::Path &path) const {
	return Common::ArchiveMemberPtr(new Common::GenericArchiveMember(path, *this));
}

Common::SeekableReadStream *ROSpriteArea::createReadStreamForMember(const Common::Path &path) const {
	if (!_stream || !_map.contains(path))
		return nullptr;

	const SpriteEntry &entry = _map[path];

	// We could use SafeSeekableSubReadStream here, but we don't really need it for most cases.
	return new Common::SeekableSubReadStream(_stream, entry.offset, entry.offset + entry.size);
}

ROSpriteDecoder *ROSpriteArea::createDecoderForMember(const Common::Path &path) const {
	Common::SeekableReadStream *stream = createReadStreamForMember(path);
	if (!stream)
		return nullptr;

	ROSpriteDecoder *decoder = new ROSpriteDecoder();
	if (!decoder->loadStream(*stream)) {
		delete decoder;
		delete stream;
		return nullptr;
	}

	delete stream;
	return decoder;
}



const byte riscos_mode_vars[50][3] = {
	{ 1, 2, 0 }, { 2, 2, 1 }, { 3, 2, 2 }, { 1, 2, 1 }, { 2, 2, 0 }, // 0-4
	{ 3, 2, 1 }, { 2, 2, 1 }, { 2, 2, 2 }, { 1, 2, 1 }, { 2, 2, 2 }, // 5-9
	{ 3, 2, 3 }, { 1, 2, 1 }, { 1, 2, 2 }, { 2, 2, 3 }, { 1, 2, 2 }, // 10-14
	{ 1, 2, 3 }, { 1, 2, 2 }, { 1, 2, 2 }, { 1, 1, 0 }, { 1, 1, 1 }, // 15-19
	{ 1, 1, 2 }, { 1, 1, 3 }, { 0, 1, 2 }, { 1, 1, 0 }, { 1, 2, 3 }, // 20-24
	{ 1, 1, 0 }, { 1, 1, 1 }, { 1, 1, 2 }, { 1, 1, 3 }, { 1, 1, 0 }, // 25-29
	{ 1, 1, 1 }, { 1, 1, 2 }, { 1, 1, 3 }, { 1, 2, 0 }, { 1, 2, 1 }, // 30-34
	{ 1, 2, 2 }, { 1, 2, 3 }, { 1, 2, 0 }, { 1, 2, 1 }, { 1, 2, 2 }, // 35-39
	{ 1, 2, 3 }, { 1, 2, 0 }, { 1, 2, 1 }, { 1, 2, 2 }, { 1, 2, 0 }, // 40-44
	{ 1, 2, 1 }, { 1, 2, 2 }, { 2, 2, 3 }, { 2, 1, 2 }, { 2, 1, 3 }  // 45-49
};

static const byte riscos_palette_2[2 * 3] = {
	0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, // 0-1
};

static const byte riscos_palette_4[4 * 3] = {
	0xFF, 0xFF, 0xFF, 0xBB, 0xBB, 0xBB, 0x55, 0x55, 0x55, 0x00, 0x00, 0x00, // 0-3
};

static const byte riscos_palette_16[16 * 3] = {
	0xFF, 0xFF, 0xFF, 0xDD, 0xDD, 0xDD, 0xBB, 0xBB, 0xBB, 0x99, 0x99, 0x99, // 0-3
	0x77, 0x77, 0x77, 0x55, 0x55, 0x55, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00, // 4-7
	0x00, 0x44, 0x99, 0xEE, 0xEE, 0x00, 0x00, 0xCC, 0x00, 0xDD, 0x00, 0x00, // 8-11
	0xEE, 0xEE, 0xBB, 0x55, 0x88, 0x00, 0xFF, 0xBB, 0x00, 0x00, 0xBB, 0xFF, // 12-15
};

static const byte riscos_palette_256[256 * 3] = {
	0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x33, 0x33, 0x33, // 0-3
	0x44, 0x00, 0x00, 0x55, 0x11, 0x11, 0x66, 0x22, 0x22, 0x77, 0x33, 0x33, // 4-7
	0x00, 0x00, 0x44, 0x11, 0x11, 0x55, 0x22, 0x22, 0x66, 0x33, 0x33, 0x77, // 8-11
	0x44, 0x00, 0x44, 0x55, 0x11, 0x55, 0x66, 0x22, 0x66, 0x77, 0x33, 0x77, // 12-15
	0x88, 0x00, 0x00, 0x99, 0x11, 0x11, 0xAA, 0x22, 0x22, 0xBB, 0x33, 0x33, // 16-19
	0xCC, 0x00, 0x00, 0xDD, 0x11, 0x11, 0xEE, 0x22, 0x22, 0xFF, 0x33, 0x33, // 20-23
	0x88, 0x00, 0x44, 0x99, 0x11, 0x55, 0xAA, 0x22, 0x66, 0xBB, 0x33, 0x77, // 24-27
	0xCC, 0x00, 0x44, 0xDD, 0x11, 0x55, 0xEE, 0x22, 0x66, 0xFF, 0x33, 0x77, // 28-31
	0x00, 0x44, 0x00, 0x11, 0x55, 0x11, 0x22, 0x66, 0x22, 0x33, 0x77, 0x33, // 32-35
	0x44, 0x44, 0x00, 0x55, 0x55, 0x11, 0x66, 0x66, 0x22, 0x77, 0x77, 0x33, // 36-39
	0x00, 0x44, 0x44, 0x11, 0x55, 0x55, 0x22, 0x66, 0x66, 0x33, 0x77, 0x77, // 40-43
	0x44, 0x44, 0x44, 0x55, 0x55, 0x55, 0x66, 0x66, 0x66, 0x77, 0x77, 0x77, // 44-47
	0x88, 0x44, 0x00, 0x99, 0x55, 0x11, 0xAA, 0x66, 0x22, 0xBB, 0x77, 0x33, // 48-51
	0xCC, 0x44, 0x00, 0xDD, 0x55, 0x11, 0xEE, 0x66, 0x22, 0xFF, 0x77, 0x33, // 52-55
	0x88, 0x44, 0x44, 0x99, 0x55, 0x55, 0xAA, 0x66, 0x66, 0xBB, 0x77, 0x77, // 56-59
	0xCC, 0x44, 0x44, 0xDD, 0x55, 0x55, 0xEE, 0x66, 0x66, 0xFF, 0x77, 0x77, // 60-63
	0x00, 0x88, 0x00, 0x11, 0x99, 0x11, 0x22, 0xAA, 0x22, 0x33, 0xBB, 0x33, // 64-67
	0x44, 0x88, 0x00, 0x55, 0x99, 0x11, 0x66, 0xAA, 0x22, 0x77, 0xBB, 0x33, // 68-71
	0x00, 0x88, 0x44, 0x11, 0x99, 0x55, 0x22, 0xAA, 0x66, 0x33, 0xBB, 0x77, // 72-75
	0x44, 0x88, 0x44, 0x55, 0x99, 0x55, 0x66, 0xAA, 0x66, 0x77, 0xBB, 0x77, // 76-79
	0x88, 0x88, 0x00, 0x99, 0x99, 0x11, 0xAA, 0xAA, 0x22, 0xBB, 0xBB, 0x33, // 80-83
	0xCC, 0x88, 0x00, 0xDD, 0x99, 0x11, 0xEE, 0xAA, 0x22, 0xFF, 0xBB, 0x33, // 84-87
	0x88, 0x88, 0x44, 0x99, 0x99, 0x55, 0xAA, 0xAA, 0x66, 0xBB, 0xBB, 0x77, // 88-91
	0xCC, 0x88, 0x44, 0xDD, 0x99, 0x55, 0xEE, 0xAA, 0x66, 0xFF, 0xBB, 0x77, // 92-95
	0x00, 0xCC, 0x00, 0x11, 0xDD, 0x11, 0x22, 0xEE, 0x22, 0x33, 0xFF, 0x33, // 96-99
	0x44, 0xCC, 0x00, 0x55, 0xDD, 0x11, 0x66, 0xEE, 0x22, 0x77, 0xFF, 0x33, // 100-103
	0x00, 0xCC, 0x44, 0x11, 0xDD, 0x55, 0x22, 0xEE, 0x66, 0x33, 0xFF, 0x77, // 104-107
	0x44, 0xCC, 0x44, 0x55, 0xDD, 0x55, 0x66, 0xEE, 0x66, 0x77, 0xFF, 0x77, // 108-111
	0x88, 0xCC, 0x00, 0x99, 0xDD, 0x11, 0xAA, 0xEE, 0x22, 0xBB, 0xFF, 0x33, // 112-115
	0xCC, 0xCC, 0x00, 0xDD, 0xDD, 0x11, 0xEE, 0xEE, 0x22, 0xFF, 0xFF, 0x33, // 116-119
	0x88, 0xCC, 0x44, 0x99, 0xDD, 0x55, 0xAA, 0xEE, 0x66, 0xBB, 0xFF, 0x77, // 120-123
	0xCC, 0xCC, 0x44, 0xDD, 0xDD, 0x55, 0xEE, 0xEE, 0x66, 0xFF, 0xFF, 0x77, // 124-127
	0x00, 0x00, 0x88, 0x11, 0x11, 0x99, 0x22, 0x22, 0xAA, 0x33, 0x33, 0xBB, // 128-131
	0x44, 0x00, 0x88, 0x55, 0x11, 0x99, 0x66, 0x22, 0xAA, 0x77, 0x33, 0xBB, // 132-135
	0x00, 0x00, 0xCC, 0x11, 0x11, 0xDD, 0x22, 0x22, 0xEE, 0x33, 0x33, 0xFF, // 136-139
	0x44, 0x00, 0xCC, 0x55, 0x11, 0xDD, 0x66, 0x22, 0xEE, 0x77, 0x33, 0xFF, // 140-143
	0x88, 0x00, 0x88, 0x99, 0x11, 0x99, 0xAA, 0x22, 0xAA, 0xBB, 0x33, 0xBB, // 144-147
	0xCC, 0x00, 0x88, 0xDD, 0x11, 0x99, 0xEE, 0x22, 0xAA, 0xFF, 0x33, 0xBB, // 148-151
	0x88, 0x00, 0xCC, 0x99, 0x11, 0xDD, 0xAA, 0x22, 0xEE, 0xBB, 0x33, 0xFF, // 152-155
	0xCC, 0x00, 0xCC, 0xDD, 0x11, 0xDD, 0xEE, 0x22, 0xEE, 0xFF, 0x33, 0xFF, // 156-159
	0x00, 0x44, 0x88, 0x11, 0x55, 0x99, 0x22, 0x66, 0xAA, 0x33, 0x77, 0xBB, // 160-163
	0x44, 0x44, 0x88, 0x55, 0x55, 0x99, 0x66, 0x66, 0xAA, 0x77, 0x77, 0xBB, // 164-167
	0x00, 0x44, 0xCC, 0x11, 0x55, 0xDD, 0x22, 0x66, 0xEE, 0x33, 0x77, 0xFF, // 168-171
	0x44, 0x44, 0xCC, 0x55, 0x55, 0xDD, 0x66, 0x66, 0xEE, 0x77, 0x77, 0xFF, // 172-175
	0x88, 0x44, 0x88, 0x99, 0x55, 0x99, 0xAA, 0x66, 0xAA, 0xBB, 0x77, 0xBB, // 176-179
	0xCC, 0x44, 0x88, 0xDD, 0x55, 0x99, 0xEE, 0x66, 0xAA, 0xFF, 0x77, 0xBB, // 180-183
	0x88, 0x44, 0xCC, 0x99, 0x55, 0xDD, 0xAA, 0x66, 0xEE, 0xBB, 0x77, 0xFF, // 184-187
	0xCC, 0x44, 0xCC, 0xDD, 0x55, 0xDD, 0xEE, 0x66, 0xEE, 0xFF, 0x77, 0xFF, // 188-191
	0x00, 0x88, 0x88, 0x11, 0x99, 0x99, 0x22, 0xAA, 0xAA, 0x33, 0xBB, 0xBB, // 192-195
	0x44, 0x88, 0x88, 0x55, 0x99, 0x99, 0x66, 0xAA, 0xAA, 0x77, 0xBB, 0xBB, // 196-199
	0x00, 0x88, 0xCC, 0x11, 0x99, 0xDD, 0x22, 0xAA, 0xEE, 0x33, 0xBB, 0xFF, // 200-203
	0x44, 0x88, 0xCC, 0x55, 0x99, 0xDD, 0x66, 0xAA, 0xEE, 0x77, 0xBB, 0xFF, // 204-207
	0x88, 0x88, 0x88, 0x99, 0x99, 0x99, 0xAA, 0xAA, 0xAA, 0xBB, 0xBB, 0xBB, // 208-211
	0xCC, 0x88, 0x88, 0xDD, 0x99, 0x99, 0xEE, 0xAA, 0xAA, 0xFF, 0xBB, 0xBB, // 212-215
	0x88, 0x88, 0xCC, 0x99, 0x99, 0xDD, 0xAA, 0xAA, 0xEE, 0xBB, 0xBB, 0xFF, // 216-219
	0xCC, 0x88, 0xCC, 0xDD, 0x99, 0xDD, 0xEE, 0xAA, 0xEE, 0xFF, 0xBB, 0xFF, // 220-223
	0x00, 0xCC, 0x88, 0x11, 0xDD, 0x99, 0x22, 0xEE, 0xAA, 0x33, 0xFF, 0xBB, // 224-227
	0x44, 0xCC, 0x88, 0x55, 0xDD, 0x99, 0x66, 0xEE, 0xAA, 0x77, 0xFF, 0xBB, // 228-231
	0x00, 0xCC, 0xCC, 0x11, 0xDD, 0xDD, 0x22, 0xEE, 0xEE, 0x33, 0xFF, 0xFF, // 232-235
	0x44, 0xCC, 0xCC, 0x55, 0xDD, 0xDD, 0x66, 0xEE, 0xEE, 0x77, 0xFF, 0xFF, // 236-239
	0x88, 0xCC, 0x88, 0x99, 0xDD, 0x99, 0xAA, 0xEE, 0xAA, 0xBB, 0xFF, 0xBB, // 240-243
	0xCC, 0xCC, 0x88, 0xDD, 0xDD, 0x99, 0xEE, 0xEE, 0xAA, 0xFF, 0xFF, 0xBB, // 244-247
	0x88, 0xCC, 0xCC, 0x99, 0xDD, 0xDD, 0xAA, 0xEE, 0xEE, 0xBB, 0xFF, 0xFF, // 248-251
	0xCC, 0xCC, 0xCC, 0xDD, 0xDD, 0xDD, 0xEE, 0xEE, 0xEE, 0xFF, 0xFF, 0xFF  // 252-255
};

const byte *const riscos_palettes[4] = {
	riscos_palette_2,
	riscos_palette_4,
	riscos_palette_16,
	riscos_palette_256,
};

} // End of namespace Image
