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

#ifndef NOOT_WIDGETS_H
#define NOOT_WIDGETS_H

#include "common/rect.h"

namespace Common {
struct KeyState;
}

namespace Noot {
class NootEngine;

class Widget {
protected:
	NootEngine *_engine;
	Common::Rect _area;
	Common::Point _lastMouse;

	bool _isDirty;

public:
	Widget(NootEngine *engine, const Common::Rect &area) : _engine(engine), _area(area), _isDirty(true) {}
	virtual ~Widget() {}

	virtual void load() = 0;
	virtual void free() = 0;

	void invalidate() { _isDirty = true; }
	bool isDirty() const { return _isDirty; }
	virtual void render() = 0;

	void handleMouseMotion(const Common::Point &mouse);
	virtual void handleMouseEnter(const Common::Point &mouse) {}
	virtual void handleMouseLeave(const Common::Point &mouse) {}
	virtual void handleKeyDown(const Common::KeyState &kbd) {}
	virtual void handleKeyUp(const Common::KeyState &kbd) {}
};

class ButtonWidget : public Widget {
private:
	Common::Path _off, _on;

	bool _hover;

	Graphics::Surface *_offSurf, *_offMask;
	Graphics::Surface *_onSurf, *_onMask;
	uint32 *_offMap, *_onMap;

public:
	ButtonWidget(NootEngine *engine, const Common::Rect &area,
                     const Common::Path &off, const Common::Path &on);
	~ButtonWidget() override;

	void load() override;
	void free() override;
	void render() override;

	void handleMouseEnter(const Common::Point &mouse) override;
	void handleMouseLeave(const Common::Point &mouse) override;
};

class InputWidget : public Widget {
private:
	Common::String _text;
	Graphics::Surface *_surface;
	uint32 _fgColour, _bgColour, _caretColour;
	uint _maxChars;

	void redraw();

public:
	InputWidget(NootEngine *engine, const Common::Rect &area, uint maxChars);
	~InputWidget() override;

	void load() override;
	void free() override;
	void render() override;

	void handleKeyDown(const Common::KeyState &kbd) override;
};

} // End of namespace Noot

#endif // NOOT_WIDGETS_H
