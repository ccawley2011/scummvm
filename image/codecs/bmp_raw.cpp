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

#include "image/codecs/bmp_raw.h"

#include "common/stream.h"
#include "common/textconsole.h"
#include "graphics/surface.h"

namespace Image {

BitmapRawDecoder::BitmapRawDecoder(int width, int height, int bitsPerPixel, bool ignoreAlpha, bool flip) : Codec(),
		_width(width), _height(height), _bitsPerPixel(bitsPerPixel), _ignoreAlpha(ignoreAlpha), _flip(flip)  {
	if (height < 0) {
		_height = -height;
		_flip = !flip;
	}
	_surface.create(_width, _height, getPixelFormat());
}

BitmapRawDecoder::~BitmapRawDecoder() {
	_surface.free();
}

const Graphics::Surface *BitmapRawDecoder::decodeFrame(Common::SeekableReadStream &stream) {
	Graphics::PixelFormat format = getPixelFormat();

	int srcPitch = _width * (_bitsPerPixel >> 3);
	int extraDataLength = (srcPitch % 4) ? 4 - (srcPitch % 4) : 0;

	if (_bitsPerPixel == 1) {
		srcPitch = (_width + 7) / 8;
		extraDataLength = (srcPitch % 2) ? 2 - (srcPitch % 2) : 0;
	} else if (_bitsPerPixel == 4) {
		srcPitch = (_width + 1) / 2;
		extraDataLength = (srcPitch % 4) ? 4 - (srcPitch % 4) : 0;
	}

	byte *dst = (byte *)_surface.getBasePtr(0, _flip ? 0 : _height - 1);
	int dstStride = _flip ? _surface.pitch : -_surface.pitch;

	if (_bitsPerPixel == 1) {
		for (int i = 0; i < _height; i++) {
			for (int j = 0; j != _width;) {
				byte color = stream.readByte();
				for (int k = 0; k < 8; k++) {
					dst[j] = (color & 0x80) >> 7;
					color <<= 1;
					j++;
					if (j == _width) {
						break;
					}
				}
			}
			stream.skip(extraDataLength);
			dst += dstStride;
		}
	} else if (_bitsPerPixel == 4) {
		for (int i = 0; i < _height; i++) {
			for (int j = 0; j < _width; j++) {
				byte color = stream.readByte();

				dst[j] = (color & 0xf0) >> 4;
				j++;

				if (j ==_width)
					break;

				dst[j] = color & 0x0f;
			}

			stream.skip(extraDataLength);
			dst += dstStride;
		}
#ifndef SCUMM_LITTLE_ENDIAN
	} else if (_bitsPerPixel == 16) {
		for (int i = 0; i < _height; i++) {
			for (int j = 0; j < _width; j++) {
				uint16 color = stream.readUint16LE();

				((uint16 *)dst)[j] = color;
			}

			stream.skip(extraDataLength);
			dst += dstStride;
		}
#endif
	} else {
		uint bpp = format.bytesPerPixel;

		for (int i = 0; i < _height; i++) {
			stream.read(dst, _width * bpp);
			stream.skip(extraDataLength);
			dst += dstStride;
		}
	}

	return &_surface;
}

Graphics::PixelFormat BitmapRawDecoder::getPixelFormat() const {
	switch (_bitsPerPixel) {
	case 1:
	case 4:
	case 8:
		return Graphics::PixelFormat::createFormatCLUT8();
	case 16:
		return Graphics::PixelFormat(2, 5, 5, 5, 0, 10, 5, 0, 0);
	case 24:
		return Graphics::PixelFormat::createFormatBGR24();
	case 32:
		return Graphics::PixelFormat::createFormatBGRA32(!_ignoreAlpha);
	default:
		break;
	}

	error("Unhandled BMP raw %dbpp", _bitsPerPixel);
}

} // End of namespace Image
