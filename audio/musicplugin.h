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

#ifndef AUDIO_MUSICPLUGIN_H
#define AUDIO_MUSICPLUGIN_H

#include "base/plugins.h"
#include "audio/mididrv.h"
#include "common/list.h"

namespace Common {
class Error;
}

class MusicPluginObject;

/**
 * @defgroup audio_musicplugin Music devices and plugins
 * @ingroup audio
 *
 * @brief API for managing music devices and music plugins.
 * @{
 */

/**
 * Description of a Music device. Used to list the devices a Music driver
 * can manage and their capabilities.
 * A device with an empty name means the default device.
 */
class MusicDevice {
public:
	MusicDevice(MusicPluginObject const *musicPlugin, Common::String name, MusicType mt);

	const Common::String &getName() const { return _name; }
	const Common::String &getMusicDriverName() const { return _musicDriverName; }
	const Common::String &getMusicDriverId() const { return _musicDriverId; }
	MusicType getMusicType() const { return _type; }

	/**
	 * Returns a user readable string that contains the name of the current
	 * device name (if it isn't the default one) and the name of the driver.
	 */
	Common::String getCompleteName();

	/**
	 * Returns a user readable string that contains the name of the current
	 * device name (if it isn't the default one) and the id of the driver.
	 */
	Common::String getCompleteId();

	MidiDriver::DeviceHandle getHandle();

private:
	Common::String _name;
	Common::String _musicDriverName;
	Common::String _musicDriverId;
	MusicType _type;
};

/** List of music devices. */
typedef Common::List<MusicDevice> MusicDevices;

/**
 * A MusicPluginObject is essentially a factory for MidiDriver instances with
 * the added ability of listing the available devices and their capabilities.
 */
class MusicPluginObject : public PluginObject {
public:
	virtual ~MusicPluginObject() {}

	/**
	 * Returns a unique string identifier which will be used to save the
	 * selected MIDI driver to the config file.
	 */
	virtual const char *getId() const = 0;

	/**
	 * Returns a list of the available devices.
	 */
	virtual MusicDevices getDevices() const = 0;

	/**
	 * Checks whether a device can actually be used. Currently this is only
	 * implemented for the MT-32 emulator to check whether the required rom
	 * files are present.  In the default implementation, the device is not
	 * available as an auto-selected device.
	 *
	 * @param hdl         MIDI device handle
	 * @param checkFlags  Bitwise OR mask of MidiDriverCheckFlags
	 * @param quiet       If true, suppress and error messages on check failure.
	 * 
	 */
	virtual bool checkDevice(MidiDriver::DeviceHandle hdl, int checkFlags, bool quiet) const { return (checkFlags & MDCK_AUTO) == 0; }

	/**
	 * Tries to instantiate a MIDI Driver instance based on the device
	 * previously detected via MidiDriver::detectDevice()
	 *
	 * @param mididriver	Pointer to a pointer which the MusicPluginObject sets
	 *				to the newly create MidiDriver, or 0 in case of an error
	 *
	* @param dev	Pointer to a device to be used then creating the driver instance.
	 *				Default value of zero for driver types without devices.
	 *
	 * @return		a Common::Error describing the error which occurred, or kNoError
	 */
	virtual Common::Error createInstance(MidiDriver **mididriver, MidiDriver::DeviceHandle = 0) const = 0;
};

/**
 * Singleton class which manages all Music plugins.
 */
class MusicManager : public Common::Singleton<MusicManager> {
private:
	friend class Common::Singleton<SingletonBaseType>;

public:
	const PluginList &getPlugins() const;
};

/** Convenience shortcut for accessing the Music manager. */
#define MusicMan MusicManager::instance()
/** @} */
#endif
