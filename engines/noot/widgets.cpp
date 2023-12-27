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

#include "noot/widgets.h"
#include "noot/noot.h"

#include "graphics/surface.h"
#include "image/rosprite.h"

namespace Noot {

void Widget::handleMouseMotion(const Common::Point &mouse) {
	if (_area.contains(mouse) && !_area.contains(_lastMouse))
		handleMouseEnter(mouse);
	else if (!_area.contains(mouse) && _area.contains(_lastMouse))
		handleMouseLeave(mouse);
	_lastMouse = mouse;
}


ButtonWidget::ButtonWidget(NootEngine *engine, const Common::Rect &area,
                           const Common::Path &off, const Common::Path &on) :
	Widget(engine, area),
	_off(off), _on(on), _hover(false),
	_offSurf(nullptr), _offMask(nullptr),
	_onSurf(nullptr), _onMask(nullptr),
	_offMap(nullptr), _onMap(nullptr) {
	load();
}

ButtonWidget::~ButtonWidget() {
	free();
}

void ButtonWidget::load() {
	free();

	const Image::ROSpriteArea *spriteArea = _engine->getSpriteArea();
	Image::ROSpriteDecoder *decoder;

	decoder = spriteArea->createDecoderForMember(_off);
	assert(decoder);
	if (decoder) {
		_offSurf = _engine->scaleSurface(decoder->getSurface(), decoder->getXEigFactor(), decoder->getYEigFactor());
		_offMask = _engine->scaleSurface(decoder->getMask(), decoder->getXEigFactor(), decoder->getYEigFactor());
		_offMap = _engine->createMap(decoder->getPalette(), decoder->getPaletteColorCount());
		delete decoder;
	}

	decoder = spriteArea->createDecoderForMember(_on);
	if (decoder) {
		_onSurf = _engine->scaleSurface(decoder->getSurface(), decoder->getXEigFactor(), decoder->getYEigFactor());
		_onMask = _engine->scaleSurface(decoder->getMask(), decoder->getXEigFactor(), decoder->getYEigFactor());
		_onMap = _engine->createMap(decoder->getPalette(), decoder->getPaletteColorCount());
		delete decoder;
	}

	invalidate();
}

void ButtonWidget::free() {
	if (_offSurf) {
		_offSurf->free();
		delete _offSurf;
		_offSurf = nullptr;
	}
	if (_onSurf) {
		_onSurf->free();
		delete _onSurf;
		_onSurf = nullptr;
	}

	if (_offMask) {
		_offMask->free();
		delete _offMask;
		_offMask = nullptr;
	}

	if (_onMask) {
		_onMask->free();
		delete _onMask;
		_onMask = nullptr;
	}

	delete[] _offMap;
	_offMap = nullptr;

	delete[] _onMap;
	_onMap = nullptr;
}

void ButtonWidget::render() {
	if (_hover) {
		if (_onSurf)
			_engine->copyToScreen(_onSurf, _onMask, _onMap, _area);
	} else {
		if (_offSurf)
			_engine->copyToScreen(_offSurf, _offMask, _offMap, _area);
	}
	_engine->drawRect(_area);

	_isDirty = false;
}

void ButtonWidget::handleMouseEnter(const Common::Point &mouse) {
	_hover = true;
	invalidate();
}

void ButtonWidget::handleMouseLeave(const Common::Point &mouse) {
	_hover = false;
	invalidate();
}

} // End of namespace Noot
