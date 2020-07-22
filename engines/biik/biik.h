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

#ifndef BIIK_BIIK_H
#define BIIK_BIIK_H

#include "common/str.h"

#include "engines/engine.h"

struct ADGameDescription;

namespace Biik {
class BiikArchive;
class BiikGUI;
class Script;

enum BiikDebugChannels {
	kDebugGeneral = 1 << 0,
	kDebugGUI     = 1 << 1
};

enum GameFileTypes {
	GAME_STARTFILE = 1 << 0,
	GAME_SPRITEFILE = 1 << 1,
	GAME_TEMPLATEFILE  = 1 << 2,
	GAME_EXEFILE = 1 << 3
};

class BiikGame : public Engine {
public:
	BiikGame(OSystem *syst, const ADGameDescription *gameDesc);
	~BiikGame();

	virtual Common::Error run() override;

	// Detection related functions
	const ADGameDescription *_gameDescription;
	const char *getGameId() const;
	Common::Platform getPlatform() const;
	uint32 getFeatures() const;
	const char *getFileName(int type) const;
	bool isBigEndian() const { return getPlatform() == Common::kPlatformMacintosh; }

	BiikArchive *_archive;
	BiikGUI *_gui;
	Script *_script;

protected:
	virtual bool hasFeature(EngineFeature f) const override;
};

} // End of namespace Biik

#endif
