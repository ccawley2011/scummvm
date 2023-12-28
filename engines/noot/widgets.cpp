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
#include "noot/animation.h"

#include "common/keyboard.h"
#include "common/stream.h"
#include "common/system.h"
#include "common/util.h"
#include "graphics/font.h"
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

	if (_engine->getDebugRects())
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


InputWidget::InputWidget(NootEngine *engine, const Common::Rect &area, uint maxChars) :
	Widget(engine, area), _maxChars(maxChars), _surface(nullptr) {
	load();
}

InputWidget::~InputWidget() {
	free();
}

void InputWidget::load() {
	free();

	_surface = new Graphics::Surface();
	_surface->create(_area.width() >> _engine->getXEigFactor(),
	                 _area.height() >> _engine->getYEigFactor(),
	                 g_system->getScreenFormat());

	_fgColour    = _engine->findBestColor(0,    0,    0);
	_bgColour    = _engine->findBestColor(0xff, 0xff, 0xff);
	_caretColour = _engine->findBestColor(0xdd, 0,    0);

	redraw();
}

void InputWidget::free() {
	if (_surface) {
		_surface->free();
		delete _surface;
		_surface = nullptr;
	}
}

void InputWidget::render() {
	if (_surface)
		_engine->copyToScreen(_surface, nullptr, nullptr, _area);
	_isDirty = false;
}

void InputWidget::redraw() {
	_surface->fillRect(Common::Rect(_surface->w, _surface->h), _bgColour);

	const Graphics::Font *font = _engine->getFont();

	uint x = 12 >> _engine->getXEigFactor();
	uint y = (_surface->h - font->getFontHeight()) / 2;

	if (!_text.empty()) {
		Common::Rect textPos = font->getBoundingBox(_text, x, y, _surface->w - (x * 2));
		font->drawString(_surface, _text, x, y, _surface->w - (x * 2), _fgColour);
		x = textPos.right;
	}

	_surface->hLine(x - 3, 1, x - 2, _caretColour);
	_surface->hLine(x + 2, 1, x + 3, _caretColour);
	_surface->setPixel(x - 1, 2, _caretColour);
	_surface->setPixel(x + 1, 2, _caretColour);
	_surface->vLine(x, 3, _surface->h - 3, _caretColour);
	_surface->setPixel(x - 1, _surface->h - 2, _caretColour);
	_surface->setPixel(x + 1, _surface->h - 2, _caretColour);
	_surface->hLine(x - 3, _surface->h - 1, x - 2, _caretColour);
	_surface->hLine(x + 2, _surface->h - 1, x + 3, _caretColour);

	invalidate();
}

void InputWidget::handleKeyDown(const Common::KeyState &kbd) {
	if (kbd.keycode == Common::KEYCODE_BACKSPACE) {
		_text.deleteLastChar();
		redraw();
	} else if (Common::isPrint(kbd.ascii)) {
		if (_text.size() < _maxChars)
			_text += kbd.ascii;
		redraw();
	}
}


AnimationWidget::AnimationWidget(NootEngine *engine, const Common::Rect &area) :
	Widget(engine, area), _animation(nullptr), _map(nullptr), _dirtyPalette(true) {
	load();
}

AnimationWidget::~AnimationWidget() {
	free();

	delete _animation;
}

bool AnimationWidget::loadStream(Common::SeekableReadStream *stream) {
	Animation *anim = new Animation();
	if (!anim->loadStream(stream)) {
		delete anim;
		return false;
	}

	delete _animation;
	_animation = anim;
	_animation->start();

	return true;
}

void AnimationWidget::load() {
	free();

	_dirtyPalette = true;

	invalidate();
}

void AnimationWidget::free() {
	delete[] _map;
	_map = nullptr;
}

bool AnimationWidget::isDirty() const {
	return Widget::isDirty() || _animation->needsUpdate();
}

void AnimationWidget::render() {
	if (!_frame || _animation->needsUpdate())
		_frame = _animation->decodeNextFrame();

	if (_frame) {
		if (_animation->hasDirtyPalette() || _dirtyPalette) {
			delete[] _map;
			_map = _engine->createMap(_animation->getPalette(), 256);
			_dirtyPalette = false;
		}

		if (_engine->getXEigFactor() == _animation->getXEigFactor() &&
		    _engine->getYEigFactor() == _animation->getYEigFactor()) {
			_engine->copyToScreen(_frame, nullptr, _map, _area);
		} else {
			Graphics::Surface *scaled = _engine->scaleSurface(_frame, _animation->getXEigFactor(), _animation->getYEigFactor());
			_engine->copyToScreen(scaled, nullptr, _map, _area);
			scaled->free();
			delete scaled;
		}

		if (_engine->getDebugRects()) {
			Common::Rect dirtyRect(_animation->getDirtyRect());
			dirtyRect.translate(_area.left, _area.top);
			dirtyRect.clip(_area);
			_engine->drawRect(_area);
			if (!dirtyRect.isEmpty())
				_engine->drawRect(dirtyRect);
		}
	}
}


TextWidget::TextWidget(NootEngine *engine, const Common::Rect &area, const Common::String &text) :
	Widget(engine, area), _text(text) {
	load();
}

TextWidget::~TextWidget() {
	free();
}

void TextWidget::load() {
	free();

	uint w = _area.width() >> _engine->getXEigFactor();
	uint h = _area.height() >> _engine->getYEigFactor();

	Common::Array<Common::String> lines;
	const Graphics::Font *font = _engine->getFont();
	font->wordWrapText(_text, w, lines);

	int y = (h - (font->getFontHeight() * lines.size())) / 2;

	for (uint i = 0; i < lines.size(); i++) {
		Line line;
		line.text = lines[i];
		line.w = font->getStringWidth(line.text);
		line.x = (w - line.w) / 2;
		line.y = y;
		_lines.push_back(line);

		y += font->getFontHeight();
	}

	_textColour = _engine->findBestColor(0, 0, 0);
}

void TextWidget::free() {
	_lines.clear();
}

void TextWidget::render() {
	Graphics::Surface *screen = _engine->lockScreen(_area);
	if (screen) {
		const Graphics::Font *font = _engine->getFont();

		for (uint i = 0; i < _lines.size(); i++) {
			font->drawString(screen, _lines[i].text, _lines[i].x, _lines[i].y, _lines[i].w, _textColour);
		}

		_engine->unlockScreen(screen);
	}
}

} // End of namespace Noot
