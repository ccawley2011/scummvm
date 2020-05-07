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

#ifndef BIIK_GUI_RISCOS_TEMPLATE_H
#define BIIK_GUI_RISCOS_TEMPLATE_H

#include "common/array.h"
#include "common/str.h"
#include "common/rect.h"

namespace Common {
class SeekableReadStream;
}

namespace Biik {

class Template {
public:
	struct IndirectedData {
		union {
			char data[12];
			struct {
				int32 nameptr;
				int32 validptr;
				int32 length;
			} indirect;
		};
		Common::String name;
		Common::String valid;
	};

	struct WindowDef {
		uint32 minx;
		uint32 miny;
		uint32 maxx;
		uint32 maxy;
		uint32 scrollx;
		uint32 scrolly;
		uint32 handleBehind;
		uint32 windowFlags;
		uint8 titleFgColour;
		uint8 titleBgColour;
		uint8 workAreaFgColour;
		uint8 workAreaBgColour;
		uint8 scrollOuterColour;
		uint8 scrollInnerColour;
		uint8 titleFocusBgColour;
		uint8 reserved;
		uint32 workAreaMinX;
		uint32 workAreaMinY;
		uint32 workAreaMaxX;
		uint32 workAreaMaxY;
		uint32 titleFlags;
		uint32 workAreaFlags;
		uint32 spriteAreaControlBlock;
		uint16 minWidth;
		uint16 minHeight;
		IndirectedData titleData;
		uint32 numIcons;
	};

	enum IconFlags {
		ICON_CONTAINS_TEXT        = 1 << 0,
		ICON_CONTAINS_SPRITE      = 1 << 1,
		ICON_HAS_BORDER           = 1 << 2,
		ICON_CENTRED_HORIZONTALLY = 1 << 3,
		ICON_CENTRED_VERTICALLY   = 1 << 4,
		ICON_HAS_BACKGROUND       = 1 << 5,
		ICON_ANTI_ALIASED_FONT    = 1 << 6,
		ICON_MANUAL_REDRAW        = 1 << 7,
		ICON_IS_INDIRECTED        = 1 << 8,
		ICON_RIGHT_ALIGN          = 1 << 9,

		ICON_IS_SELECTED          = 1 << 21,
		ICON_IS_SHADED            = 1 << 22,
		ICON_IS_DELETED           = 1 << 23,

		ICON_FOREGROUND_COLOUR    = 0xF << 24,
		ICON_BACKGROUND_COLOUR    = 0xF << 28,

		ICON_FONT_HANDLE          = 0xFF << 24
	};


	struct IconDef {
		Common::Rect rect;
		uint32 minx;
		uint32 miny;
		uint32 maxx;
		uint32 maxy;
		uint32 flags;
		IndirectedData iconData;
	};

	struct Index {
		uint32 offset;
		uint32 size;
		uint32 entryType;
		char identifier[12];
	};

	Template();
	virtual ~Template();

	void destroy();
	bool open(const Common::String &fileName, Common::String name);
	bool open(Common::SeekableReadStream *stream, Common::String name);
	const WindowDef *getWindowDef() const { return &_windowDef; }
	const Common::Array<IconDef> &getIconDefs() const { return _iconDefs; }

private:
	int _id;
	Common::Array<Index> _index;
	WindowDef _windowDef;
	Common::Array<IconDef> _iconDefs;

	bool loadIndex(Common::SeekableReadStream *stream);
	void loadWindowDef(Common::SeekableReadStream *stream);
	void loadIconDef(Common::SeekableReadStream *stream);
	IndirectedData readIndirectedData(Common::SeekableReadStream *stream, uint32 flags);
	Common::String readString(Common::SeekableReadStream *stream, int32 offset, size_t bufLength);
	Common::String readString(Common::SeekableReadStream *stream, int32 offset);

};

#define GET_ICON_FOREGROUND_COLOUR(x) ((x & Template::ICON_FOREGROUND_COLOUR) >> 24)
#define GET_ICON_BACKGROUND_COLOUR(x) ((x & Template::ICON_BACKGROUND_COLOUR) >> 28)

} // End of namespace Biik

#endif
