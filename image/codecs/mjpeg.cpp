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

// Based on LGPL MJPEG/AVI to JPEG/JFIF conversion code from libav
// Copyright (c) 2010 Adrian Daerr and Nicolas George
// That in turn was adapted from mjpeg2jpeg.c, with original copyright:
// Paris 2010 Adrian Daerr, public domain

#include "common/memstream.h"
#include "common/system.h"
#include "common/textconsole.h"
#include "graphics/surface.h"
#include "image/jpeg.h"

#include "image/codecs/mjpeg.h"

namespace Common {
class SeekableReadStream;
}

namespace Image {

MJPEGDecoder::MJPEGDecoder() : Codec() {
	_pixelFormat = getDefaultYUVFormat();

	_surface = 0;
	_accuracy = CodecAccuracy::Default;
}

MJPEGDecoder::~MJPEGDecoder() {
	if (_surface) {
		_surface->free();
		delete _surface;
	}
}

// Header to be inserted
static const byte s_jpegHeader[] = {
	0xff, 0xd8,                     // SOI
	0xff, 0xe0,                     // APP0
	0x00, 0x10,                     // APP0 header size (including
	                                // this field, but excluding preceding)
	'J', 'F', 'I', 'F', 0x00,       // ID string 'JFIF\0'
	0x01, 0x01,                     // version
	0x00,                           // bits per type
	0x00, 0x00,                     // X density
	0x00, 0x00,                     // Y density
	0x00,                           // X thumbnail size
	0x00
};

enum {
	DHT_SEGMENT_SIZE = 420
};

static const byte s_dhtSegmentHead[] = { 0xFF, 0xC4, 0x01, 0xA2, 0x00 };
static const byte s_dhtSegmentFrag[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
	0x0a, 0x0b, 0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Set up the standard Huffman tables (cf. JPEG standard section K.3)
// IMPORTANT: these are only valid for 8-bit data precision!
static const byte s_mjpegBitsDCLuminance[17] = {
	/* 0-base */ 0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0
};

static const byte s_mjpegValDC[12] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
};

#if 0
static const byte s_mjpegBitsDCChrominance[17] = {
	/* 0-base */ 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0
};
#endif

static const byte s_mjpegBitsACLuminance[17] = {
	/* 0-base */ 0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d
};

static const byte s_mjpegValACLuminance[] = {
	0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
	0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
	0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
	0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
	0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
	0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
	0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
	0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
	0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
	0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
	0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
	0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
	0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
	0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
	0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
	0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
	0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
	0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa
};

static const byte s_mjpegBitsACChrominance[17] = {
	/* 0-base */ 0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77
};

static const byte s_mjpegValACChrominance[] = {
	0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
	0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
	0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
	0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
	0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
	0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
	0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
	0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
	0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
	0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
	0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
	0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
	0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
	0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
	0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
	0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
	0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
	0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
	0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa
};

const Graphics::Surface *MJPEGDecoder::decodeFrame(Common::SeekableReadStream &stream) {
	// We need to reconstruct an actual JPEG stream here, then feed it to the JPEG decoder
	// Yes, this is a pain.

	stream.readUint32BE(); // Skip nonsense JPEG header
	uint16 inputSkip = stream.readUint16BE() + 4;
	uint32 tag = stream.readUint32BE();

	if (tag != MKTAG('A', 'V', 'I', '1')) {
		warning("Invalid MJPEG tag found");
		return 0;
	}

	uint32 outputSize = stream.size() - inputSkip + sizeof(s_jpegHeader) + DHT_SEGMENT_SIZE;
	byte *data = (byte *)malloc(outputSize);

	if (!data) {
		warning("Failed to allocate data for MJPEG conversion");
		return 0;
	}

	// Copy the header
	memcpy(data, s_jpegHeader, sizeof(s_jpegHeader));
	uint32 dataOffset = sizeof(s_jpegHeader);

	// Write the fake DHT segment
	memcpy(data + dataOffset, s_dhtSegmentHead, sizeof(s_dhtSegmentHead));
	dataOffset += sizeof(s_dhtSegmentHead);
	memcpy(data + dataOffset, s_mjpegBitsDCLuminance + 1, 16);
	dataOffset += 16;
	memcpy(data + dataOffset, s_dhtSegmentFrag, sizeof(s_dhtSegmentFrag));
	dataOffset += sizeof(s_dhtSegmentFrag);
	memcpy(data + dataOffset, s_mjpegValDC, 12);
	dataOffset += 12;
	data[dataOffset++] = 0x10;
	memcpy(data + dataOffset, s_mjpegBitsACLuminance + 1, 16);
	dataOffset += 16;
	memcpy(data + dataOffset, s_mjpegValACLuminance, 162);
	dataOffset += 162;
	data[dataOffset++] = 0x11;
	memcpy(data + dataOffset, s_mjpegBitsACChrominance + 1, 16);
	dataOffset += 16;
	memcpy(data + dataOffset, s_mjpegValACChrominance, 162);
	dataOffset += 162;

	// Write the actual data
	stream.seek(inputSkip);
	stream.read(data + dataOffset, stream.size() - inputSkip);

	Common::MemoryReadStream convertedStream(data, outputSize, DisposeAfterUse::YES);
	JPEGDecoder jpeg;
	jpeg.setCodecAccuracy(_accuracy);
	jpeg.setOutputPixelFormat(_pixelFormat);

	if (!jpeg.loadStream(convertedStream)) {
		warning("Failed to decode MJPEG frame");
		return 0;
	}

	if (_surface) {
		_surface->free();
		delete _surface;
	}

	_surface = new Graphics::Surface();
	_surface->copyFrom(*jpeg.getSurface());

	assert(_surface->format == _pixelFormat);

	return _surface;
}

void MJPEGDecoder::setCodecAccuracy(CodecAccuracy accuracy) {
	_accuracy = accuracy;
}

} // End of namespace Image
