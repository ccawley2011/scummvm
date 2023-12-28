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

#include "noot/animation.h"
#include "noot/decompress.h"

#include "common/memstream.h"
#include "common/stream.h"

#include "graphics/surface.h"
#include "image/rosprite.h"

namespace Noot {

bool Animation::loadStream(Common::SeekableReadStream *stream) {
	close();

	AnimationTrack *track = new AnimationTrack();
	if (!track->loadStream(stream)) {
		delete track;
		return false;
	}

	addTrack(track);
	return true;
}

uint Animation::getXEigFactor() const {
	const Track *track = getTrack(0);

	if (track)
		return ((const AnimationTrack *)track)->getXEigFactor();

	return 0;
}

uint Animation::getYEigFactor() const {
	const Track *track = getTrack(0);

	if (track)
		return ((const AnimationTrack *)track)->getYEigFactor();

	return 0;
}

Common::Rect Animation::getDirtyRect() const {
	const Track *track = getTrack(0);

	if (track)
		return ((const AnimationTrack *)track)->getDirtyRect();

	return Common::Rect();
}

Animation::AnimationTrack::AnimationTrack() :
	_fileStream(nullptr),
	_spriteMem(nullptr),
	_spriteMemSize(0),
	_frameDelays(nullptr),
	_frameOffsets(nullptr),
	_initialFrame(true),
	_curFrame(0),
	_nextFrameStartTime(0),
	_reversed(false) {
	_spriteDecoder = new Image::ROSpriteDecoder();
}

Animation::AnimationTrack::~AnimationTrack() {
	delete _spriteDecoder;
	delete _fileStream;
	delete[] _spriteMem;
	delete[] _frameDelays;
	delete[] _frameOffsets;
}

bool Animation::AnimationTrack::loadStream(Common::SeekableReadStream *stream) {
	_flags      = stream->readUint32LE();
	_frameCount = stream->readUint32LE();
	/* uint32 unknown2 = */ stream->readUint32LE();
	/* uint32 unknown3 = */ stream->readUint32LE();
	_frameDelays = new uint32[_frameCount];
	for (uint32 i = 0; i < _frameCount; i++)
		_frameDelays[i] = stream->readUint32LE() * 10;

	_frameOffsets = new int64[_frameCount];
	_dirtyRects = new Common::Rect[_frameCount]();

	for (uint32 i = 0; i < _frameCount; i++) {
		_frameOffsets[i] = stream->pos();
		uint32 size   = stream->readUint32LE();
		/* uint32 method = */ stream->readUint32LE();
		int32 left    = stream->readSint32LE();
		int32 bottom  = stream->readSint32LE();
		int32 right   = stream->readSint32LE();
		int32 top     = stream->readSint32LE();
		// Animation 22 in Farm has reversed left and right coordinates.
		_dirtyRects[i] = Common::Rect(MIN(left, right), bottom, MAX(left, right), top);

		if (i != _frameCount - 1)
			stream->seek(_frameOffsets[i] + size);
	}

	stream->seek(_frameOffsets[0]);
	if (!readNextFrame(stream))
		return false;
	_initialFrame = true;

	memcpy(_palette, _spriteDecoder->getPalette(), _spriteDecoder->getPaletteColorCount() * 3);
	_dirtyPalette = true;

	_fileStream = stream;
	return true;
}

bool Animation::AnimationTrack::endOfTrack() const {
	if (_frameCount == 1 && _curFrame == 1)
		return true;
	return false;
}

bool Animation::AnimationTrack::rewind() {
	_fileStream->seek(_frameOffsets[0]);

	_curFrame = 0;
	_nextFrameStartTime = 0;
	return true;
}

uint16 Animation::AnimationTrack::getWidth() const {
	const Graphics::Surface *surface = _spriteDecoder->getSurface();
	return surface->w;
}

uint16 Animation::AnimationTrack::getHeight() const {
	const Graphics::Surface *surface = _spriteDecoder->getSurface();
	return surface->h;
}

Graphics::PixelFormat Animation::AnimationTrack::getPixelFormat() const {
	const Graphics::Surface *surface = _spriteDecoder->getSurface();
	return surface->format;
}

uint Animation::AnimationTrack::getXEigFactor() const {
	return _spriteDecoder->getXEigFactor();
}

uint Animation::AnimationTrack::getYEigFactor() const {
	return _spriteDecoder->getYEigFactor();
}

const Graphics::Surface *Animation::AnimationTrack::decodeNextFrame() {
	/* The first frame was already decoded during loadStrean() */
	if (!_initialFrame) {
		_fileStream->seek(_frameOffsets[_curFrame]);
		if (!readNextFrame(_fileStream))
			warning("Failed to read next frame");
	} else {
		_initialFrame = false;
	}


	if (!_reversed) {
		_dirtyRect = _dirtyRects[_curFrame];
		_nextFrameStartTime += _frameDelays[_curFrame];

		if (_curFrame >= _frameCount - 1) {
			// The demo versions of Dweek and Farm contain
			// yoyo animations with only one frame.
			if (_flags & kFlagYoyo && _frameCount > 1)
				_reversed = true;
			else
				_curFrame = 1;
		} else {
			_curFrame++;
		}
	} else {
		_dirtyRect = _dirtyRects[_curFrame - 1];
		_nextFrameStartTime += _frameDelays[_curFrame - 1];

		if (_curFrame <= 1)
			_reversed = false;
		else
			_curFrame--;
	}

	return _spriteDecoder->getSurface();
}

bool Animation::AnimationTrack::readNextFrame(Common::SeekableReadStream *stream) {
	uint32 size   = stream->readUint32LE();
	uint32 method = stream->readUint32LE();
	/* int32 left    = */ stream->readSint32LE();
	/* int32 bottom  = */ stream->readSint32LE();
	/* int32 right   = */ stream->readSint32LE();
	/* int32 top     = */ stream->readSint32LE();

	uint32 compressedSize = size - 24;
	byte *compressedData = new byte[compressedSize];
	stream->read(compressedData, compressedSize);

	byte *uncompressedData = nullptr;
	uint32 uncompressedSize = 0;

	uint32 spriteOffset, dataOffset;

	switch (method) {
	case 2:
		if (!decompressLZW(compressedData, compressedSize, uncompressedData, uncompressedSize)) {
			delete[] compressedData;
			return false;
		}

		delete[] compressedData;
		delete[] _spriteMem;
		_spriteMem = uncompressedData;
		_spriteMemSize = uncompressedSize;
		break;
	case 13:
		if (!decompressLZW(compressedData, compressedSize, uncompressedData, uncompressedSize)) {
			delete[] compressedData;
			return false;
		}

		spriteOffset = READ_LE_UINT32(_spriteMem + 8);
		dataOffset = spriteOffset + READ_LE_UINT32(_spriteMem + spriteOffset + 32);

		if (!decompressRLEXOR(uncompressedData, uncompressedSize,
		                      _spriteMem + dataOffset, _spriteMemSize - dataOffset)) {
			delete[] uncompressedData;
			delete[] compressedData;
			return false;
		}

		delete[] uncompressedData;
		delete[] compressedData;
		break;
	default:
		warning("Unsupported compression method %d", method);
		delete[] compressedData;
		return false;
	}

	spriteOffset = READ_LE_UINT32(_spriteMem + 8);
	Common::MemoryReadStream memstream(_spriteMem + spriteOffset,
	                                   _spriteMemSize - spriteOffset,
	                                   DisposeAfterUse::NO);
	return _spriteDecoder->loadStream(memstream);
}

} // End of namespace Noot
