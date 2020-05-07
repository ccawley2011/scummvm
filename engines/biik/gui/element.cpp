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

#include "biik/gui/element.h"

#include "graphics/font.h"
#include "graphics/managed_surface.h"
#include "graphics/surface.h"

#include "image/rosprite.h"

namespace Biik {

Element::Element(Graphics::ManagedSurface &parent, Common::Rect rect, int id) {
	_target.create(parent, rect);
	_id = id;
	_mask = nullptr;
}

Element::~Element() {
	removeSurface();
}

bool Element::contains(const Common::Point &point) const {
	Common::Rect rect = _target.getBounds();
	rect.moveTo(_target.getOffsetFromOwner());
	return rect.contains(point);
}

void Element::setSurface(Image::ROSpriteDecoder *decoder) {
	removeSurface();

	const Graphics::Surface *imgSurface = decoder->getSurface();
	const Graphics::Surface *imgMask = decoder->getMask();

	_rect = Common::Rect::center(_target.getBounds().width() / 2, _target.getBounds().height() / 2,
	                             (imgSurface->w << decoder->getXEigFactor()) >> 1,
	                             (imgSurface->h << decoder->getYEigFactor()) >> 1);

	_surface.create(imgSurface->w, imgSurface->h, imgSurface->format);
	_surface.blitFrom(*imgSurface);
	_surface.setPalette(decoder->getPalette(), decoder->getPaletteStartIndex(), decoder->getPaletteColorCount());

	if (imgMask) {
		_mask = new Graphics::Surface();
		_mask->copyFrom(*imgMask);
	}
}

void Element::setText(const Graphics::Font *font, Common::String text, uint32 bgColour, uint32 fgColour) {
	_textRect = Common::Rect::center(_target.getBounds().width() / 2, _target.getBounds().height() / 2,
	                                 font->getStringWidth(text), font->getFontHeight());
	_font = font;
	_text = text;
	_textBgColour = bgColour;
	_textFgColour = fgColour;
}

void Element::removeSurface() {
	if (_mask) {
		delete _mask;
		_mask = nullptr;
	}
}

void Element::update() {
	_target.frameRect(_target.getBounds(), 0);

	_target.transBlitFrom(_surface, Common::Rect(_surface.w, _surface.h), _rect, 0, false, 0, 0xFF, _mask, true);

	_target.fillRect(_textRect, _textBgColour);
	_font->drawString(&_target, _text, _textRect.left, _textRect.top, _textRect.width(), _textFgColour);
}

} // End of namespace Biik
