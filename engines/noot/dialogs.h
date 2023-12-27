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

#ifndef NOOT_DIALOGS_H
#define NOOT_DIALOGS_H


#include "common/events.h"
#include "common/str.h"
#include "common/ustr.h"

#include "engines/dialogs.h"

#include "gui/dialog.h"
#include "gui/widget.h"

namespace GUI {
class PopUpWidget;
}

namespace Noot {

class NootEngine;

struct NootResolution {
	const char *label;
	uint xeig, yeig;
};

class NootOptionsWidget : public GUI::OptionsContainerWidget {
public:
	NootOptionsWidget(GuiObject *boss, const Common::String &name, const Common::String &domain);
	~NootOptionsWidget() override;

	// OptionsContainerWidget API
	void load() override;
	bool save() override;

	static void registerDefaultSettings(const Common::String &target);

	static const NootResolution resolutions[];

private:
	// OptionsContainerWidget API
	void defineLayout(GUI::ThemeEval &layouts, const Common::String &layoutName, const Common::String &overlayedLayout) const override;

	GUI::PopUpWidget *_resolutionPopUp;
};

} // End of namespace Noot

#endif
