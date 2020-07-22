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

#include "biik/sound.h"
#include "biik/biik.h"

#include "audio/decoders/wave.h"
#include "audio/audiostream.h"

#include "common/file.h"
#include "common/tokenizer.h"

namespace Biik {

Sound::Sound(BiikGame *vm, const Common::FSNode &waveDir)
	: _vm(vm),
	  _mixer(_vm->_mixer),
	  _waveDir(waveDir),
	  _currentDir(waveDir) {
}

Sound::~Sound() {
	killWaveFile();
}

bool Sound::playWaveFile(const Common::String &filename) {
	Common::FSNode node = convertPath(filename, _currentDir);
	Common::SeekableReadStream *file = node.createReadStream();
	if (!file)
		return false;

	Audio::RewindableAudioStream *audioStream = Audio::makeWAVStream(file, DisposeAfterUse::YES);
	Audio::AudioStream *stream = audioStream;
	killWaveFile();
	_mixer->playStream(Audio::Mixer::kSFXSoundType, &_waveHandle, stream, -1, Audio::Mixer::kMaxChannelVolume);

	return true;
}

void Sound::killWaveFile() {
	if (_mixer->isSoundHandleActive(_waveHandle))
		_mixer->stopHandle(_waveHandle);
}

void Sound::setCurrentDir(const Common::String &dir) {
	_currentDir = convertPath(dir, _waveDir);
}

Common::String Sound::convertPath(Common::String str) {
	for (uint32 i = 0; i < str.size(); ++i) {
		if (str[i] == '/')
			str.setChar('.', i);
	}

	return str;
}

Common::FSNode Sound::convertPath(const Common::String &str, Common::FSNode node) {
	Common::StringTokenizer tokenizer(str, ".");
	while (!tokenizer.empty()) {
		node = node.getChild(convertPath(tokenizer.nextToken()));
	}

	return node;
}

} // End of namespace Biik
