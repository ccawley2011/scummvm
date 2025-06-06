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

// This file was mostly copied from SCI with light modifications.

#include "dgds/dgds.h"
#include "dgds/sound/midiparser_sci.h"
#include "dgds/sound/drivers/mididriver.h"

namespace Dgds {

static const int nMidiParams[] = { 2, 2, 2, 2, 1, 1, 2, 0 };

enum SciMidiCommands {
	kSetSignalLoop = 0x7F,
	kEndOfTrack = 0xFC,
	kSetReverb = 0x50,
	kMidiHold = 0x52,
	kUpdateCue = 0x60,
	kResetOnPause = 0x4C
};

//  MidiParser_SCI
//
MidiParser_SCI::MidiParser_SCI(SciMusic *music) :
	MidiParser() {
	_music = music;
	// mididata contains delta in 1/60th second
	// values of ppqn and tempo are found experimentally and may be wrong
	_ppqn = 1;
	MidiParser::setTempo(16667);

	_track = nullptr;
	_pSnd = nullptr;
	_loopTick = 0;
	_masterVolume = 15;
	_volume = 127;

	_resetOnPause = false;

	_mainThreadCalled = false;

	resetStateTracking();
}

MidiParser_SCI::~MidiParser_SCI() {
	MidiParser_SCI::unloadMusic();
	// we do this, so that MidiParser won't be able to call his own ::allNotesOff()
	//  this one would affect all channels and we can't let that happen
	_driver = nullptr;
}

void MidiParser_SCI::mainThreadBegin() {
	assert(!_mainThreadCalled);
	_mainThreadCalled = true;
}

void MidiParser_SCI::mainThreadEnd() {
	assert(_mainThreadCalled);
	_mainThreadCalled = false;
}

bool MidiParser_SCI::loadMusic(SoundResource::Track *track, MusicEntry *psnd, int channelFilterMask) {
	unloadMusic();
	_track = track;
	_pSnd = psnd;

	for (int i = 0; i < 16; i++) {
		_channelUsed[i] = false;
		_channelVolume[i] = 127;
		_channelRemap[i] = -1;
	}

	midiMixChannels();

	_numTracks = 1;
	_numSubtracks[0] = 1;
	_tracks[0][0] = const_cast<byte *>(_mixedData->data());
	if (_pSnd)
		setTrack(0);
	_loopTick = 0;

	return true;
}

byte MidiParser_SCI::midiGetNextChannel(long ticker) {
	byte curr = 0xFF;
	long closest = ticker + 1000000, next = 0;

	for (int i = 0; i < _track->channelCount; i++) {
		if (_track->channels[i].time == -1) // channel ended
			continue;
		SoundResource::Channel *curChannel = &_track->channels[i];
		if (curChannel->curPos >= curChannel->data.size())
			continue;
		next = curChannel->data[curChannel->curPos]; // when the next event should occur
		if (next == 0xF8) // 0xF8 means 240 ticks delay
			next = 240;
		next += _track->channels[i].time;
		if (next < closest) {
			curr = i;
			closest = next;
		}
	}

	return curr;
}

static inline bool validateNextRead(const SoundResource::Channel *channel) {
	if (channel->data.size() <= channel->curPos) {
		warning("Unexpected end of %s. Music may sound wrong due to game resource corruption", channel->data.name().c_str());
		return false;
	}
	return true;
}

void MidiParser_SCI::midiMixChannels() {
	int totalSize = 0;

	for (int i = 0; i < _track->channelCount; i++) {
		_track->channels[i].time = 0;
		_track->channels[i].prev = 0;
		_track->channels[i].curPos = 0;
		// Ignore the digital channel data, if it exists - it's not MIDI data
		if (i == _track->digitalChannelNr)
			continue;
		totalSize += _track->channels[i].data.size();
	}

	SciSpan<byte> outData = _mixedData->allocate(totalSize * 2, Common::String::format("mixed sound.%d", _pSnd ? _pSnd->resourceId : -1)); // FIXME: creates overhead and still may be not enough to hold all data

	long ticker = 0;
	byte channelNr, curDelta;
	byte midiCommand = 0, midiParam, globalPrev = 0;
	long newDelta;
	SoundResource::Channel *channel;
	bool breakOut = false;

	while ((channelNr = midiGetNextChannel(ticker)) != 0xFF) { // there is still an active channel
		channel = &_track->channels[channelNr];
		if (!validateNextRead(channel))
			break;
		curDelta = channel->data[channel->curPos++];
		channel->time += (curDelta == 0xF8 ? 240 : curDelta); // when the command is supposed to occur
		if (curDelta == 0xF8)
			continue;
		newDelta = channel->time - ticker;
		ticker += newDelta;

		if (channelNr == _track->digitalChannelNr)
			continue;
		if (!validateNextRead(channel))
			break;
		midiCommand = channel->data[channel->curPos++];
		if (midiCommand != kEndOfTrack) {
			// Write delta
			while (newDelta > 240) {
				*outData++ = 0xF8;
				newDelta -= 240;
			}
			*outData++ = (byte)newDelta;
		}

		// Write command
		switch (midiCommand) {
		case 0xF0: // sysEx
			*outData++ = midiCommand;
			do {
				if (!validateNextRead(channel)) {
					breakOut = true;
					break;
				}
				midiParam = channel->data[channel->curPos++];
				*outData++ = midiParam;
			} while (midiParam != 0xF7);
			break;
		case kEndOfTrack: // end of channel
			channel->time = -1;
			break;
		default: // MIDI command
			if (midiCommand & 0x80) {
				if (!validateNextRead(channel)) {
					breakOut = true;
					break;
				}
				midiParam = channel->data[channel->curPos++];
			} else {// running status
				midiParam = midiCommand;
				midiCommand = channel->prev;
			}

			// remember which channel got used for channel remapping
			byte midiChannel = midiCommand & 0xF;
			_channelUsed[midiChannel] = true;

			if (midiCommand != globalPrev)
				*outData++ = midiCommand;
			*outData++ = midiParam;
			if (nMidiParams[(midiCommand >> 4) - 8] == 2) {
				if (!validateNextRead(channel)) {
					breakOut = true;
					break;
				}
				*outData++ = channel->data[channel->curPos++];
			}
			channel->prev = midiCommand;
			globalPrev = midiCommand;
		}

		if (breakOut)
			break;
	}

	// Insert stop event
	*outData++ = 0;    // Delta
	*outData++ = 0xFF; // Meta event
	*outData++ = 0x2F; // End of track (EOT)
	*outData++ = 0x00;
	*outData++ = 0x00;
}

#if 0
static inline bool validateNextRead(const SciSpan<const byte> &channelData, const SciSpan<const byte>::size_type size = 1) {
	if (channelData.size() < size) {
		warning("Unexpected end of %s. Music may sound wrong due to game resource corruption", channelData.name().c_str());
		return false;
	}
	return true;
}
#endif

void MidiParser_SCI::resetStateTracking() {
	for (int i = 0; i < 16; ++i) {
		ChannelState &s = _channelState[i];
		s._modWheel = 0;
		s._pan = 64;
		s._patch = 0; // TODO: Initialize properly (from data in LoadMusic?)
		s._note = -1;
		s._sustain = false;
		s._pitchWheel = 0x2000;
		s._voices = 0;

		_channelVolume[i] = 127;
	}
}

void MidiParser_SCI::initTrack() {
	return;
}

void MidiParser_SCI::sendInitCommands() {
	resetStateTracking();

	// reset our "global" volume
	_volume = 127;

	// Set initial voice count
	if (_pSnd) {
		for (int i = 0; i < _track->channelCount; ++i) {
			byte voiceCount = _track->channels[i].poly;
			byte num = _track->channels[i].number;
			// TODO: Should we skip the control channel?
			sendToDriver(0xB0 | num, 0x4B, voiceCount);
		}
	}

	// Reset all the parameters of the channels used by this song
	for (int i = 0; i < 16; ++i) {
		if (_channelUsed[i]) {
			sendToDriver(0xB0 | i, 0x07, 127);	// Reset volume to maximum
			sendToDriver(0xB0 | i, 0x0A, 64);	// Reset panning to center
			sendToDriver(0xB0 | i, 0x40, 0);	// Reset hold pedal to none
			sendToDriver(0xE0 | i,    0, 64);	// Reset pitch wheel to center
		}
	}
}

void MidiParser_SCI::unloadMusic() {
	if (_pSnd) {
		resetTracking();
		allNotesOff();
		// Pending track init commands have to be removed from the queue,
		// since the sound thread will otherwise continue to try executing these.
		_music->removeTrackInitCommandsFromQueue(_pSnd);
	}
	_numTracks = 0;
	_pSnd = nullptr;
	_track = nullptr;
	_activeTrack = 255;
	_resetOnPause = false;
	_mixedData.clear();
}

// this is used for scripts sending midi commands to us. we verify in that case that the channel is actually
//  used, so that channel remapping will work as well and then send them on
void MidiParser_SCI::sendFromScriptToDriver(uint32 midi) {
	byte midiChannel = midi & 0xf;

	if (!_channelUsed[midiChannel]) {
		// trying to send to an unused channel
		// this happens for cmdSendMidi at least in sq1vga right at the start, it's a script issue
		return;
	}

	if ((midi & 0xFFF0) == 0x4EB0 && DgdsEngine::getInstance()->getGameId() != GID_DRAGON) {
		// We have to handle this here instead of inside the trackState() method (which handles the input from
		// the actual midi data). The mute command when sent from the script is independent from the mute
		// command sent by the actual midi data. The script mute is stacked on the high nibble, while the midi
		// data mute is stored on the low nibble. So the script cannot undo a mute set by the midi data and vice
		// versa.
		byte channel = midi & 0xf;
		bool op = (midi >> 16) & 0x7f;
		uint8 m = _pSnd->_chan[channel]._mute;

		if (op && _pSnd->_chan[channel]._mute < 0xF0)
			_pSnd->_chan[channel]._mute += 0x10;
		else if (!op && _pSnd->_chan[channel]._mute >= 0x10)
			_pSnd->_chan[channel]._mute -= 0x10;

		if (_pSnd->_chan[channel]._mute != m) {
			// CHECKME: Should we directly call remapChannels() if _mainThreadCalled?
			_music->needsRemap();
			debugC(2, kDebugLevelSound, "Dynamic mute change (arg = %d, mainThread = %d)", m, _mainThreadCalled);
		}

		return;
	}

	sendToDriver(midi);
}

void MidiParser_SCI::sendToDriver(uint32 midi) {
	byte midiChannel = midi & 0xf;

	// State tracking
	if (!_pSnd->_chan[midiChannel]._dontMap)
		trackState(midi);

	if ((midi & 0xFFF0) == 0x4EB0 && DgdsEngine::getInstance()->getGameId() != GID_DRAGON) {
		// Mute. Handled in trackState()/sendFromScriptToDriver().
		return;
	}

	if ((midi & 0xFFF0) == 0x07B0) {
		// someone trying to set channel volume?
		int channelVolume = (midi >> 16) & 0xFF;
		// Adjust volume accordingly to current local volume
		channelVolume = channelVolume * _volume / 127;
		midi = (midi & 0xFFFF) | ((channelVolume & 0xFF) << 16);
	}

	// Channel remapping
	uint8 msg = (midi & 0xF0);
	int16 realChannel = _channelRemap[midiChannel];
	if (_pSnd->_chan[midiChannel]._dontMap) {
		// The dontMap channel is supposed to have limited access, if the device channel is already in use.
		// It probably won't happen, but the original does these checks...
		if (!_music->isDeviceChannelMapped(midiChannel) || (msg != 0xB0 && msg != 0xC0 && msg != 0xE0))
			realChannel = midiChannel;
	}

	if (realChannel == -1)
		return;

	midi = (midi & 0xFFFFFFF0) | realChannel;
	sendToDriver_raw(midi);
}

void MidiParser_SCI::sendToDriver_raw(uint32 midi) {
	if (_mainThreadCalled)
		_music->putMidiCommandInQueue(midi);
	else
		_driver->send(midi);
}

void MidiParser_SCI::trackState(uint32 b) {
	// We keep track of most of the state of a midi channel, so we can
	// at any time reset the device to the current state, even if the
	// channel has been temporarily disabled due to remapping.

	byte command = b & 0xf0;
	byte channel = b & 0xf;
	byte op1 = (b >> 8) & 0x7f;
	byte op2 = (b >> 16) & 0x7f;

	ChannelState &s = _channelState[channel];

	switch (command) {
	case 0x90:
		if (op2 != 0) {
			// note on
			s._note = op1;
			break;
		}
		// else, fall-through
	case 0x80:
		// note off
		if (s._note == op1)
			s._note = -1;
		break;
	case 0xB0:
		// control change
		switch (op1) {
		case 0x01: // mod wheel
			s._modWheel = op2;
			break;
		case 0x07: // channel volume
			_channelVolume[channel] = op2;
			break;
		case 0x0A: // pan
			s._pan = op2;
			break;
		case 0x40: // sustain
			s._sustain = (op2 != 0);
			break;
		case 0x4B: // voices
			if (s._voices != op2) {
				// CHECKME: Should we directly call remapChannels() if _mainThreadCalled?
				debugC(2, kDebugLevelSound, "Dynamic voice change (%d to %d)", s._voices, op2);
				_music->needsRemap();
			}
			s._voices = op2;
			_pSnd->_chan[channel]._voices = op2; // Also sync our MusicEntry
			break;
		case 0x4E: // mute
			// This is channel mute only for HOC+.
			if (DgdsEngine::getInstance()->getGameId() != GID_DRAGON) {
				// This is handled slightly differently than what we do in sendFromScriptToDriver(). The script mute is stacked
				// on the high nibble, while the midi data mute (this one here) is stored on the low nibble. So the script cannot
				// undo a mute set by the midi data and vice versa.
				uint8 m = (_pSnd->_chan[channel]._mute & 0xf0) | (op2 & 1);
				if (_pSnd->_chan[channel]._mute != m) {
					_pSnd->_chan[channel]._mute = m;
					// CHECKME: Should we directly call remapChannels() if _mainThreadCalled?
					_music->needsRemap();
					debugC(2, kDebugLevelSound, "Dynamic mute change (arg = %d, mainThread = %d)", m, _mainThreadCalled);
				}
			}
			break;
		default:
			break;
		}
		break;
	case 0xC0:
		// program change
		s._patch = op1;
		break;
	case 0xE0:
		// pitchwheel
		s._pitchWheel = (op2 << 7) | op1;
		break;
	default:
		break;
	}
}

void MidiParser_SCI::parseNextEvent(EventInfo &info) {
	byte *playPos = _position._subtracks[0]._playPos;

	info.start = playPos;
	info.delta = 0;
	while (*playPos == 0xF8) {
		info.delta += 240;
		playPos++;
	}
	info.delta += *(playPos++);

	// Process the next info.
	if ((playPos[0] & 0xF0) >= 0x80)
		info.event = *(playPos++);
	else
		info.event = _position._subtracks[0]._runningStatus;
	if (info.event < 0x80) {
		_position._subtracks[0]._playPos = playPos;
		return;
	}

	_position._subtracks[0]._runningStatus = info.event;
	switch (info.command()) {
	case 0xC:
	case 0xD:
		info.basic.param1 = *(playPos++);
		info.basic.param2 = 0;
		break;

	case 0xB:
		info.basic.param1 = *(playPos++);
		info.basic.param2 = *(playPos++);
		info.length = 0;
		break;

	case 0x8:
	case 0x9:
	case 0xA:
	case 0xE:
		info.basic.param1 = *(playPos++);
		info.basic.param2 = *(playPos++);
		if (info.command() == 0x9 && info.basic.param2 == 0) {
			// NoteOn with param2==0 is a NoteOff
			info.event = info.channel() | 0x80;
		}
		info.length = 0;
		break;

	case 0xF: // System Common, Meta or SysEx event
		switch (info.event & 0x0F) {
		case 0x2: // Song Position Pointer
			info.basic.param1 = *(playPos++);
			info.basic.param2 = *(playPos++);
			break;

		case 0x3: // Song Select
			info.basic.param1 = *(playPos++);
			info.basic.param2 = 0;
			break;

		case 0x6:
		case 0x8:
		case 0xA:
		case 0xB:
		case 0xC:
		case 0xE:
			info.basic.param1 = info.basic.param2 = 0;
			break;

		case 0x0: // SysEx
			info.length = readVLQ(playPos);
			info.ext.data = playPos;
			playPos += info.length;
			break;

		case 0xF: // META event
			info.ext.type = *(playPos++);
			info.length = readVLQ(playPos);
			info.ext.data = playPos;
			playPos += info.length;
			break;
		default:
			warning(
					"MidiParser_SCI::parseNextEvent: Unsupported event code %x",
					info.event);
		} // // System Common, Meta or SysEx event

	default:
		break;
	}// switch (info.command())

	_position._subtracks[0]._playPos = playPos;
}

bool MidiParser_SCI::processEvent(const EventInfo &info, bool fireEvents) {
	if (!fireEvents) {
		// We don't do any processing that should be done while skipping events
		return MidiParser::processEvent(info, fireEvents);
	}

	switch (info.command()) {
	case 0xC:
		if (info.channel() == 0xF) {// SCI special case
			if (info.basic.param1 == kSetSignalLoop) {
				_loopTick = _position._lastEventTick + info.delta;
				return true;
			}

			// At least in kq5/french&mac the first scene in the intro has
			// a song that sets signal to 4 immediately on tick 0. Signal
			// isn't set at that point by sierra sci and it would cause the
			// castle daventry text to get immediately removed, so we
			// currently filter it. Sierra SCI ignores them as well at that
			// time. However, this filtering should only be performed for
			// SCI1 and newer games. Signaling is done differently in SCI0
			// though, so ignoring these signals in SCI0 games will result
			// in glitches (e.g. the intro of LB1 Amiga gets stuck - bug
			// #5693). Refer to MusicEntry::setSignal() in sound/music.cpp.
			// FIXME: SSCI doesn't start playing at the very beginning
			// of the stream, but at a fixed location a few commands later.
			// That is probably why this signal isn't triggered
			// immediately there.
			bool skipSignal = false;
			if (!_position._playTick) {
				skipSignal = true;
			}
			if (!skipSignal) {
				if (!_jumpingToTick) {
					_pSnd->setSignal(info.basic.param1);
					debugC(4, kDebugLevelSound, "signal %04x", info.basic.param1);
				}
			}

			// Done with this event.
			return true;
		}

		// Break to let parent handle the rest.
		break;
	case 0xB:
		// Reference for some events:
		// https://wiki.scummvm.org/index.php/SCI/Specifications/Sound/SCI0_Resource_Format#Status_Reference
		// Handle common special events
		switch (info.basic.param1) {
		case kSetReverb:
			if (info.basic.param2 == 127)		// Set global reverb instead
				_pSnd->reverb = _music->getGlobalReverb();
			else
				_pSnd->reverb = info.basic.param2;

			((MidiPlayer *)_driver)->setReverb(_pSnd->reverb);
			break;
		default:
			break;
		}

		// Handle events sent to the SCI special channel (15)
		if (info.channel() == 0xF) {
			switch (info.basic.param1) {
			case kSetReverb:
				// Already handled above
				return true;
			case kMidiHold:
				// Check if the hold ID marker is the same as the hold ID
				// marker set for that song by cmdSetSoundHold.
				// If it is, loop back, but don't stop notes when jumping.
				if (info.basic.param2 == _pSnd->hold) {
					jumpToTick(_loopTick, false, false);
					// Done with this event.
					return true;
				}
				return true;
			case kUpdateCue:
				if (!_jumpingToTick) {
					_pSnd->dataInc += 1;
					debugC(4, "datainc %04x", 1);

				}
				return true;
			case kResetOnPause:
				_resetOnPause = info.basic.param2;
				return true;
			// Unhandled SCI commands
			case 0x46: // LSL3 - binoculars
			case 0x61: // Iceman (AdLib?)
			case 0x73: // Hoyle
			case 0xD1: // KQ4, when riding the unicorn
				// Obscure SCI commands - ignored
				return true;
			// Standard MIDI commands
			case 0x01:	// mod wheel
			case 0x04:	// foot controller
			case 0x07:	// channel volume
			case 0x0A:	// pan
			case 0x0B:	// expression
			case 0x40:	// sustain
			case 0x79:	// reset all
			case 0x7B:	// notes off
				// These are all handled by the music driver, so ignore them
				break;
			case 0x4B:	// voice mapping
				// TODO: is any support for this needed at the MIDI parser level?
				warning("Unhanded SCI MIDI command 0x%x - voice mapping (parameter %d)", info.basic.param1, info.basic.param2);
				return true;
			default:
				warning("Unhandled SCI MIDI command 0x%x (parameter %d)", info.basic.param1, info.basic.param2);
				return true;
			}

		}

		// Break to let parent handle the rest.
		break;
	case 0xF: // META event
		if (info.ext.type == 0x2F) {// end of track reached
			// QFG3 abuses the hold flag. Its scripts call kDoSoundSetHold,
			// but sometimes there's no hold marker in the associated songs
			// (e.g. song 110, during the intro). The original interpreter
			// treats this case as an infinite loop (bug #5744).
			if (_pSnd->loop || _pSnd->hold > 0) {
				// Change from SCI: Don't stop current notes on loop.
				if (!jumpToTick(_loopTick, false, false)) {
					// Jumping to the specified tick failed. This means play
					// position is still at the end of the MIDI data. Stop the
					// sound to prevent reading out of bounds.
					warning("Jump to tick at end of looping sound failed! Stopping sound.");
					_music->soundStop(_pSnd);
				}

				// Done with this event.
				return true;

			} else {
				_pSnd->setSignal(0xffff); // SIGNAL_OFFSET

				debugC(4, kDebugLevelSound, "signal EOT");
			}
		}

		// Break to let parent handle the rest.
		break;

	default:
		// Break to let parent handle the rest.
		break;
	}


	// Let parent handle the rest
	return MidiParser::processEvent(info, fireEvents);
}

byte MidiParser_SCI::getSongReverb() {
	assert(_track);

	for (int i = 0; i < _track->channelCount; i++) {
		SoundResource::Channel &channel = _track->channels[i];
		// Peek ahead in the control channel to get the default reverb setting
		if (channel.number == 15 && channel.data.size() >= 7)
			return channel.data[6];
	}

	return 127;
}

void MidiParser_SCI::allNotesOff() {
	if (!_driver)
		return;

	int i, j;

	// Turn off all active notes
	for (i = 0; i < 128; ++i) {
		for (j = 0; j < 16; ++j) {
			if ((_activeNotes[i] & (1 << j)) && (_channelRemap[j] != -1)){
				sendToDriver(0x80 | j, i, 0);
			}
		}
	}

	// Turn off all hanging notes
	for (i = 0; i < ARRAYSIZE(_hangingNotes); i++) {
		byte midiChannel = _hangingNotes[i].channel;
		if ((_hangingNotes[i].timeLeft) && (_channelRemap[midiChannel] != -1)) {
			sendToDriver(0x80 | midiChannel, _hangingNotes[i].note, 0);
			_hangingNotes[i].timeLeft = 0;
		}
	}
	_hangingNotesCount = 0;

	// To be sure, send an "All Note Off" event (but not all MIDI devices
	// support this...).

	for (i = 0; i < 16; ++i) {
		if (_channelRemap[i] != -1) {
			sendToDriver(0xB0 | i, 0x7b, 0); // All notes off
			sendToDriver(0xB0 | i, 0x40, 0); // Also send a sustain off event (bug #5524)
		}
	}

	memset(_activeNotes, 0, sizeof(_activeNotes));
}

void MidiParser_SCI::setMasterVolume(byte masterVolume) {
	assert(masterVolume <= MUSIC_MASTERVOLUME_MAX);
	_masterVolume = masterVolume;
	((MidiPlayer *)_driver)->setVolume(masterVolume);
}

void MidiParser_SCI::setVolume(byte volume) {
	assert(volume <= MUSIC_VOLUME_MAX);
	_volume = volume;

	for (int i = 0; i < 15; i++)
		if (_channelRemap[i] != -1)
			sendToDriver(0xB0 + i, 7, _channelVolume[i]);
}

void MidiParser_SCI::remapChannel(int channel, int devChannel) {
	if (_channelRemap[channel] == devChannel)
		return;

	_channelRemap[channel] = devChannel;

	if (devChannel == -1)
		return;

//	debug(1, "  restoring state: channel %d on devChannel %d", channel, devChannel);

	// restore state
	ChannelState &s = _channelState[channel];

	int channelVolume = _channelVolume[channel];
	channelVolume = (channelVolume * _volume / 127) & 0xFF;
	byte pitch1 = s._pitchWheel & 0x7F;
	byte pitch2 = (s._pitchWheel >> 7) & 0x7F;

	sendToDriver_raw(0x0040B0 | devChannel); // sustain off
	sendToDriver_raw(0x004BB0 | devChannel | (s._voices << 16));
	sendToDriver_raw(0x0000C0 | devChannel | (s._patch << 8));
	sendToDriver_raw(0x0007B0 | devChannel | (channelVolume << 16));
	sendToDriver_raw(0x000AB0 | devChannel | (s._pan << 16));
	sendToDriver_raw(0x0001B0 | devChannel | (s._modWheel << 16));
	sendToDriver_raw(0x0040B0 | devChannel | (s._sustain ? 0x7F0000 : 0));
	sendToDriver_raw(0x0000E0 | devChannel | (pitch1 << 8) | (pitch2 << 16));

	// CHECKME: Some SSCI version send a control change 0x4E with s._note as
	// parameter.
	// We need to investigate how (and if) drivers should act on this.
	// Related: controller 0x4E is used for 'mute' in the midiparser.
	// This could be a bug in SSCI that went unnoticed because few (or no?)
	// drivers implement controller 0x4E

	// NB: The line below is _not_ valid since s._note can be 0xFF.
	// SSCI handles this out of band in the driver interface.
	// sendToDriver_raw(0x004EB0 | devChannel | (s._note << 16);
}

} // End of namespace Dgds
