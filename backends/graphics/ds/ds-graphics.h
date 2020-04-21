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

#ifndef BACKENDS_GRAPHICS_DS_H
#define BACKENDS_GRAPHICS_DS_H

#include "backends/graphics/graphics.h"

namespace DS {
class Background;

class DSGraphicsManager : public GraphicsManager {
public:
	DSGraphicsManager();
	virtual ~DSGraphicsManager();

	bool hasFeature(OSystem::Feature f) const override { return false; }
	void setFeatureState(OSystem::Feature f, bool enable) override {}
	bool getFeatureState(OSystem::Feature f) const override { return false; }

	void initSize(uint width, uint height, const Graphics::PixelFormat *format = NULL) override {}
	virtual int getScreenChangeID() const override { return 0; }

	void beginGFXTransaction() override {}
	OSystem::TransactionError endGFXTransaction() override { return OSystem::kTransactionSuccess; }

	int16 getHeight() const override { return 0; }
	int16 getWidth() const override { return 0; }
	void setPalette(const byte *colors, uint start, uint num) override {}
	void grabPalette(byte *colors, uint start, uint num) const override {}
	void copyRectToScreen(const void *buf, int pitch, int x, int y, int w, int h) override {}
	Graphics::Surface *lockScreen() override { return NULL; }
	void unlockScreen() override {}
	void fillScreen(uint32 col) override {}
	void updateScreen() override;
	void setShakePos(int shakeXOffset, int shakeYOffset) override {}
	void setFocusRectangle(const Common::Rect& rect) override {}
	void clearFocusRectangle() override {}

	void showOverlay() override;
	void hideOverlay() override;
	Graphics::PixelFormat getOverlayFormat() const override;
	void clearOverlay() override;
	void grabOverlay(void *buf, int pitch) const override;
	void copyRectToOverlay(const void *buf, int pitch, int x, int y, int w, int h) override;
	int16 getOverlayHeight() const override;
	int16 getOverlayWidth() const override;

	bool showMouse(bool visible) override { return !visible; }
	void warpMouse(int x, int y) override {}
	void setMouseCursor(const void *buf, uint w, uint h, int hotspotX, int hotspotY, uint32 keycolor, bool dontScale = false, const Graphics::PixelFormat *format = NULL) override {}
	void setCursorPalette(const byte *colors, uint start, uint num) override {}

private:
	Background *_overlay;
};

} // End of namespace DS

#endif
