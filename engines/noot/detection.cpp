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

#include "engines/advancedDetector.h"

static const PlainGameDescriptor nootGames[] = {
	{ "granny", "Granny's Garden" },
	{ "dweek", "Dweek" },
	{ "dweep", "Dweep" },
	{ "farm", "The Farm" },
	{ "howitworks", "It Works Like This..." },
	{ "playroom", "The Playroom" },
	{ "power", "Power" },
	{ "noot", "NootBook" },
	{ 0, 0 }
};

#include "noot/detection_tables.h"

class NootMetaEngineDetection : public AdvancedMetaEngineDetection {
public:
	NootMetaEngineDetection() : AdvancedMetaEngineDetection(Noot::gameDescriptions,
		sizeof(ADGameDescription), nootGames) {
		_guiOptions = GUIO3(GUIO_NOMIDI, GUIO_NOSPEECH, GUIO_NOSFX);
	}

	virtual const char *getName() const override {
		return "noot";
	}

	virtual const char *getEngineName() const override {
		return "Noot";
	}

	virtual const char *getOriginalCopyright() const override {
		return "Noot (C) 1992-1993 4Mation Educational Resources Ltd";
	}
};

REGISTER_PLUGIN_STATIC(NOOT_DETECTION, PLUGIN_TYPE_ENGINE_DETECTION, NootMetaEngineDetection);
