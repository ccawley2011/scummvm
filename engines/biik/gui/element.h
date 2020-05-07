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

#ifndef BIIK_GUI_ELEMENT_H
#define BIIK_GUI_ELEMENT_H

#include "common/array.h"
#include "common/rect.h"

#include "graphics/managed_surface.h"

namespace Graphics {
class Font;
class Surface;
}

namespace Image {
class ROSpriteDecoder;
}

namespace Biik {

class Element {
public:
	Element(Graphics::ManagedSurface &parent, Common::Rect rect, int id);
	~Element();

	int getID() const { return _id; }
	bool contains(const Common::Point &point) const;
	void setSurface(Image::ROSpriteDecoder *decoder);
	void removeSurface();
	void setText(const Graphics::Font *font, Common::String text, uint32 bgColour = 255, uint32 fgColour = 0);
	void update();

private:
	Graphics::ManagedSurface _surface, _target;
	Graphics::Surface *_mask;
	const Graphics::Font *_font;
	Common::String _text;
	Common::Rect _rect, _textRect;
	uint32 _textBgColour, _textFgColour;
	int _id;
};

typedef Common::Array<Element *> ElementArray;

} // End of namespace Biik

#endif
