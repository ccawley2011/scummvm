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

#ifndef BIIK_DETECTION_H
#define BIIK_DETECTION_H

namespace Biik {

enum GameFileTypes {
	GAME_STARTFILE = 1 << 0,
	GAME_SPRITEFILE = 1 << 1,
	GAME_TEMPLATEFILE  = 1 << 2,
	GAME_EXEFILE = 1 << 3
};

} // End of namespace Biik

#endif // BIIK_DETECTION_H
