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

#ifndef NOOT_ANIMATION_H
#define NOOT_ANIMATION_H

#include "video/video_decoder.h"

#include "common/rect.h"

namespace Image {
class ROSpriteDecoder;
}

namespace Noot {

class Animation : public Video::VideoDecoder {
public:
	Animation() {}
	~Animation() override { close(); }

	bool loadStream(Common::SeekableReadStream *stream) override;

	uint getXEigFactor() const;
	uint getYEigFactor() const;
	Common::Rect getDirtyRect() const;
	Common::Rect getLastDirtyRect() const;
	uint16 getPaletteColorCount() const;

protected:
	class AnimationTrack : public VideoTrack {
	public:
		AnimationTrack();
		~AnimationTrack();

		bool loadStream(Common::SeekableReadStream *stream);

		bool endOfTrack() const override;
		bool isRewindable() const override { return true; }
		bool rewind() override;

		uint16 getWidth() const override;
		uint16 getHeight() const override;
		Graphics::PixelFormat getPixelFormat() const override;
		uint getXEigFactor() const;
		uint getYEigFactor() const;
		Common::Rect getDirtyRect() const { return _dirtyRect; }
		Common::Rect getLastDirtyRect() const { return _lastDirtyRect; }

		int getCurFrame() const override { return _curFrame; }
		int getFrameCount() const override { return _frameCount; }
		uint32 getNextFrameStartTime() const override { return _nextFrameStartTime; }
		const Graphics::Surface *decodeNextFrame() override;
		const byte *getPalette() const override { _dirtyPalette = false; return _palette; }
		uint16 getPaletteColorCount() const;
		bool hasDirtyPalette() const override { return _dirtyPalette; }

	protected:
		enum {
			kFlagYoyo = (1 << 0),
			kFlagUnknown1 = (1 << 1),
			kFlagUnknown2 = (1 << 2)
		};

		Common::SeekableReadStream *_fileStream;

		Image::ROSpriteDecoder *_spriteDecoder;
		byte *_spriteMem;
		uint32 _spriteMemSize;

		uint _curFrame;
		bool _initialFrame;
		int64 *_frameOffsets;

		byte _palette[256 * 3];
		mutable bool _dirtyPalette;

		Common::Rect _dirtyRect, _lastDirtyRect;
		Common::Rect *_dirtyRects;

		uint32 _flags;
		uint32 _frameCount;
		uint32 *_frameDelays;
		uint32 _nextFrameStartTime;
		bool _reversed;

		bool readNextFrame(Common::SeekableReadStream *stream);
	};
};

} // End of namespace Noot

#endif // NOOT_ANIMATION_H
