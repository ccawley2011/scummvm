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

// Allow use of stuff in <time.h>
#define FORBIDDEN_SYMBOL_EXCEPTION_time_h

// Disable printf override in common/forbidden.h to avoid
// clashes with log.h from the Android SDK.
// That header file uses
//   __attribute__ ((format(printf, 3, 4)))
// which gets messed up by our override mechanism; this could
// be avoided by either changing the Android SDK to use the equally
// legal and valid
//   __attribute__ ((format(printf, 3, 4)))
// or by refining our printf override to use a varadic macro
// (which then wouldn't be portable, though).
// Anyway, for now we just disable the printf override globally
// for the Android port
#define FORBIDDEN_SYMBOL_EXCEPTION_printf

#include "backends/mixer/android/android-mixer.h"
#include "common/savefile.h"

#include "backends/platform/android/android.h"
#include "backends/platform/android/jni-android.h"

AndroidMixerManager::AndroidMixerManager(int audio_sample_rate, int audio_buffer_size) : MixerManager(),
	_audio_sample_rate(audio_sample_rate),
	_audio_buffer_size(audio_buffer_size) {

	// initial value of zero!
	sem_init(&_audio_pause_sem, 0, 0);
}

AndroidMixerManager::~AndroidMixerManager() {
	_audio_thread_exit = true;
	pthread_join(_audio_thread, 0);

	sem_destroy(&_audio_pause_sem);
}

void AndroidMixerManager::init() {
	_mixer = new Audio::MixerImpl(_audio_sample_rate);
	assert(_mixer);
	_mixer->setReady(true);

	_audio_thread_exit = false;
	pthread_create(&_audio_thread, 0, audioThreadFunc, this);
}

void AndroidMixerManager::suspendAudio() {
	_audioSuspended = true;
}

int AndroidMixerManager::resumeAudio() {
	if (!_audioSuspended)
		return -2;
	sem_post(&_audio_pause_sem);
	_audioSuspended = false;
	return 0;
}

void *AndroidMixerManager::audioThreadFunc(void *arg) {
	JNI::attachThread();

	AndroidMixerManager *this_ = (AndroidMixerManager *)arg;
	Audio::MixerImpl *mixer = this_->_mixer;

	uint buf_size = this_->_audio_buffer_size;

	JNIEnv *env = JNI::getEnv();

	jbyteArray bufa = env->NewByteArray(buf_size);

	bool paused = true;

	int offset, left, written, i;

	struct timespec tv_delay;
	tv_delay.tv_sec = 0;
	tv_delay.tv_nsec = 20 * 1000 * 1000;

	uint msecs_full = buf_size * 1000 / (mixer->getOutputRate() * 2 * 2);

	struct timespec tv_full;
	tv_full.tv_sec = 0;
	tv_full.tv_nsec = msecs_full * 1000 * 1000;

	uint silence_count = 33;

	while (!this_->_audio_thread_exit) {
		if (this_->_audioSuspended) {
			JNI::setAudioStop();

			paused = true;
			silence_count = 33;

			LOGD("audio thread going to sleep");
			sem_wait(&this_->_audio_pause_sem);
			LOGD("audio thread woke up");
		}

		byte *buf = (byte *)env->GetPrimitiveArrayCritical(bufa, 0);
		assert(buf);

		int samples = mixer->mixCallback(buf, buf_size);

		bool silence = samples < 1;

		// looks stupid, and it is, but currently there's no way to detect
		// silence-only buffers from the mixer
		if (!silence) {
			silence = true;

			for (i = 0; i < samples; i += 2)
				// SID streams constant crap
				if (READ_UINT16(buf + i) > 32) {
					silence = false;
					break;
				}
		}

		env->ReleasePrimitiveArrayCritical(bufa, buf, 0);

		if (silence) {
			if (!paused)
				silence_count++;

			// only pause after a while to prevent toggle mania
			if (silence_count > 32) {
				if (!paused) {
					LOGD("AudioTrack pause");

					JNI::setAudioPause();
					paused = true;
				}

				nanosleep(&tv_full, 0);

				continue;
			}
		}

		if (paused) {
			LOGD("AudioTrack play");

			JNI::setAudioPlay();
			paused = false;

			silence_count = 0;
		}

		offset = 0;
		left = buf_size;
		written = 0;

		while (left > 0) {
			written = JNI::writeAudio(env, bufa, offset, left);

			if (written < 0) {
				LOGE("AudioTrack error: %d", written);
				break;
			}

			// buffer full
			if (written < left)
				nanosleep(&tv_delay, 0);

			offset += written;
			left -= written;
		}

		if (written < 0)
			break;

		// prepare the next buffer, and run into the blocking AudioTrack.write
	}

	JNI::setAudioStop();

	env->DeleteLocalRef(bufa);

	JNI::detachThread();

	return 0;
}
