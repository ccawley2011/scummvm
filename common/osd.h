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

#ifndef COMMON_OSD_H
#define COMMON_OSD_H

#include "common/rect.h"

namespace Common {

/**
 * @defgroup common_osd On Screen Display
 * @ingroup common
 *
 * @brief API for managing the queue of On Screen Display (OSD) icons.
 * @{
 */

class OSDIcon {
public:
	OSDIcon() : _alpha(255), _align(kAlignCentre) {}
	virtual ~OSDIcon() {}

	virtual uint getWidth() const = 0;
	virtual uint getHeight() const = 0;

	virtual void setAlpha(uint8 alpha) { _alpha = alpha; }
	uint8 getAlpha() const { return _alpha; }

	enum Alignment {
		kAlignTopLeft,
		kAlignTopCentre,
		kAlignTopRight,

		kAlignCentreLeft,
		kAlignCentre,
		kAlignCentreRight,

		kAlignBottomLeft,
		kAlignBottomCentre,
		kAlignBottomRight
	};

	virtual void setAlignment(Alignment align) { _align = align; }
	Alignment getAlignment() const { return _align; }

protected:
	uint8 _alpha;
	Alignment _align;

	Common::Point getPosition(const Common::Rect &bounds) const {
		int16 x, y;

		switch (_align) {
		case kAlignTopLeft:
		case kAlignCentreLeft:
		case kAlignBottomLeft:
			x = bounds.left;
			break;
		case kAlignTopRight:
		case kAlignCentreRight:
		case kAlignBottomRight:
			x = bounds.right - getWidth();
			break;
		case kAlignTopCentre:
		case kAlignBottomCentre:
		case kAlignCentre:
		default:
			x = bounds.left + ((bounds.width() - getWidth()) / 2);
			break;
		}

		switch (_align) {
		case kAlignTopLeft:
		case kAlignTopCentre:
		case kAlignTopRight:
			y = bounds.top;
			break;
		case kAlignBottomLeft:
		case kAlignBottomCentre:
		case kAlignBottomRight:
			y = bounds.bottom - getHeight();
			break;
		case kAlignCentreLeft:
		case kAlignCentreRight:
		case kAlignCentre:
		default:
			y = bounds.top + ((bounds.height() - getHeight()) / 2);
			break;
		}

		return Common::Point(x, y);
	}
};

/** @} */

} // End of namespace Common

#endif
