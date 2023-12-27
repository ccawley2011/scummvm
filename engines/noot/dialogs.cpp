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

#include "noot/dialogs.h"

#include "gui/gui-manager.h"
#include "gui/message.h"
#include "gui/saveload.h"
#include "gui/ThemeEval.h"
#include "gui/widget.h"
#include "gui/widgets/popup.h"

#include "common/system.h"
#include "common/translation.h"

namespace Noot {

const NootResolution NootOptionsWidget::resolutions[] = {
	{ _s("Very high (1280x960)"), 0, 0 },
	{ _s("High (640x480)"),       1, 1 },
	{ _s("Medium (640x240)"),     1, 2 },
	{ _s("Low (320x240)"),        2, 2 },
};

NootOptionsWidget::NootOptionsWidget(GuiObject *boss, const Common::String &name, const Common::String &domain) :
		OptionsContainerWidget(boss, name, "NootGameOptionsDialog", false, domain),
		_resolutionPopUp(nullptr) {

	GUI::StaticTextWidget *resolutionCaption = new GUI::StaticTextWidget(widgetsBoss(), "NootGameOptionsDialog.ResolutionDesc", _("Resolution:"));
	resolutionCaption->setAlign(Graphics::kTextAlignRight);

	_resolutionPopUp = new GUI::PopUpWidget(widgetsBoss(), "NootGameOptionsDialog.Resolution");
	for (int i = 0; i < ARRAYSIZE(resolutions); i++)
		_resolutionPopUp->appendEntry(_(resolutions[i].label), i);
}

NootOptionsWidget::~NootOptionsWidget() {
}

void NootOptionsWidget::defineLayout(GUI::ThemeEval &layouts, const Common::String &layoutName, const Common::String &overlayedLayout) const {
	layouts.addDialog(layoutName, overlayedLayout)
	            .addLayout(GUI::ThemeLayout::kLayoutVertical)
	                .addPadding(16, 16, 16, 16)
	                .addLayout(GUI::ThemeLayout::kLayoutHorizontal)
	                    .addPadding(0, 0, 0, 0)
	                    .addWidget("ResolutionDesc", "OptionsLabel")
	                    .addWidget("Resolution", "PopUp")
	                .closeLayout()
	            .closeLayout()
	        .closeDialog();
}

void NootOptionsWidget::load() {
	if (_resolutionPopUp) {
		_resolutionPopUp->setSelectedTag(ConfMan.getInt("resolution", _domain));
	}
}

bool NootOptionsWidget::save() {
	if (_resolutionPopUp) {
		ConfMan.setInt("resolution", _resolutionPopUp->getSelectedTag(), _domain);
	}

	return true;
}

void NootOptionsWidget::registerDefaultSettings(const Common::String &target) {
#ifdef USE_HIGHRES
	ConfMan.registerDefault("resolution", 1);
#else
	ConfMan.registerDefault("resolution", 3);
#endif
}

} // End of namespace Noot
