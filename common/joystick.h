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

#ifndef COMMON_JOYSTICK_H
#define COMMON_JOYSTICK_H

namespace Common {

enum JoyCode {
	JOYCODE_INVALID = 0,

	JOYCODE_A       = 1,
	JOYCODE_B       = 2,
	JOYCODE_X       = 3,
	JOYCODE_Y       = 4,
	JOYCODE_LB      = 5,
	JOYCODE_RB      = 6,
	JOYCODE_BACK    = 7,
	JOYCODE_START   = 8,
	JOYCODE_LSTICK  = 9,
	JOYCODE_RSTICK  = 10,
	JOYCODE_GUIDE   = 11,
	JOYCODE_UP      = 12,
	JOYCODE_DOWN    = 13,
	JOYCODE_LEFT    = 14,
	JOYCODE_RIGHT   = 15
};

enum JoyAxis {
	JOYAXIS_LEFTX,
	JOYAXIS_LEFTY,
	JOYAXIS_LEFTTRIGGER,
	JOYAXIS_RIGHTX,
	JOYAXIS_RIGHTY,
	JOYAXIS_RIGHTTRIGGER
};

enum JoyHat {
	JOYHAT_CENTERED,
	JOYHAT_LEFTUP,
	JOYHAT_LEFT,
	JOYHAT_LEFTDOWN,
	JOYHAT_RIGHTUP,
	JOYHAT_RIGHT,
	JOYHAT_RIGHTDOWN,
	JOYHAT_UP,
	JOYHAT_DOWN
};

} // End of namespace Common

#endif
