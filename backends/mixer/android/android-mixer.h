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

#ifndef BACKENDS_MIXER_ANDROID_H
#define BACKENDS_MIXER_ANDROID_H

#if defined(__ANDROID__)

#include "backends/mixer/mixer.h"

#include <pthread.h>
#include <semaphore.h>

/**
 * Android mixer manager using the AudioTrack API.
 */
class AndroidMixerManager : public MixerManager {
public:
	AndroidMixerManager(int audio_sample_rate, int audio_buffer_size);
	virtual ~AndroidMixerManager();

	virtual void init();

	virtual void suspendAudio();
	virtual int resumeAudio();

private:
	int _audio_sample_rate;
	int _audio_buffer_size;

	bool _audio_thread_exit;
	sem_t _audio_pause_sem;
	pthread_t _audio_thread;
	static void *audioThreadFunc(void *arg);
};

#endif

#endif
