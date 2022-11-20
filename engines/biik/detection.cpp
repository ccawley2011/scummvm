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

#include "biik/detection.h"
#include "engines/advancedDetector.h"

static const PlainGameDescriptor biikGames[] = {
	{"darryl", "Darryl the Dragon"},
	{"dinosaur", "Dinosaur Discovery"},
	{"flossy", "Explore with Flossy the Frog"},
	{"betsi", "Betsi the Tudor Dog"},
	{"mouse", "A Mouse in Holland"},
	{"guardians", "Guardians of the Greenwood"},
	{"puppy", "Patch the Puppy"},
	{"finditfixit", "Find It, Fix It"},
	{0, 0}
};

#include "biik/detection_tables.h"

static const char *const directoryGlobs[] = {
	"!Guardians",
	"mode21",
	"BETSI",
	"data",
	"Puppy",
	0
};

class BiikMetaEngineDetection : public AdvancedMetaEngineDetection {
public:
	BiikMetaEngineDetection() : AdvancedMetaEngineDetection(Biik::gameDescriptions, sizeof(ADGameDescription), biikGames) {
		_maxScanDepth = 3;
		_directoryGlobs = directoryGlobs;
	}

	virtual const char *getName() const override {
		return "biik";
	}

	virtual const char *getEngineName() const override {
		return "Biik Engine";
	}

	virtual const char *getOriginalCopyright() const override {
		return "Biik Engine (C) 1993-1997 4Mation Educational Resources Ltd";
	}
};

REGISTER_PLUGIN_STATIC(BIIK_DETECTION, PLUGIN_TYPE_ENGINE_DETECTION, BiikMetaEngineDetection);
