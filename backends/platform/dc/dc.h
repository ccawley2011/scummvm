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

#include "backends/base-backend.h"
#include <graphics/surface.h>
#include <graphics/palette.h>
#include <ronin/soundcommon.h>
#include "backends/timer/default/default-timer.h"
#include "backends/audiocd/default/default-audiocd.h"
#include "backends/fs/fs-factory.h"
#include "audio/mixer_intern.h"
#include "common/language.h"
#include "common/platform.h"
#ifdef DYNAMIC_MODULES
#include "backends/plugins/dynamic-plugin.h"
#endif

#define NUM_BUFFERS 4
#define SOUND_BUFFER_SHIFT 3

class Interactive
{
 public:
  virtual int key(int k, byte &shiftFlags) = 0;
  virtual void mouse(int x, int y) = 0;
  virtual ~Interactive() = 0;
};
inline Interactive::~Interactive() { }

#include "softkbd.h"

class DCHardware {
 private:
  static void dc_init_hardware();
 protected:
  DCHardware() { dc_init_hardware(); }
};

class DCCDManager final : public DefaultAudioCDManager {
public:
	// Poll cdrom status
	// Returns true if cd audio is playing
	bool isPlaying() const override;

	// Play cdrom audio track
	bool play(int track, int numLoops, int startFrame, int duration, bool onlyEmulate = false,
		Audio::Mixer::SoundType soundType = Audio::Mixer::kMusicSoundType) override;

	// Stop cdrom audio track
	void stop() override;
};

class OSystem_Dreamcast final : private DCHardware, public EventsBaseBackend, public PaletteManager, public FilesystemFactory
#ifdef DYNAMIC_MODULES
  , public FilePluginProvider
#endif
 {

 public:
  OSystem_Dreamcast();

  virtual void initBackend() override;

  // Determine whether the backend supports the specified feature.
  virtual bool hasFeature(Feature f) override;

  // En-/disable the specified feature.
  virtual void setFeatureState(Feature f, bool enable) override;

  // Query the state of the specified feature.
  virtual bool getFeatureState(Feature f) override;

  // Set colors of the palette
  virtual PaletteManager *getPaletteManager() override { return this; }
protected:
	// PaletteManager API
  virtual void setPalette(const byte *colors, uint start, uint num) override;
  virtual void grabPalette(byte *colors, uint start, uint num) const override;

public:

  // Determine the pixel format currently in use for screen rendering.
  virtual Graphics::PixelFormat getScreenFormat() const override;

  // Returns a list of all pixel formats supported by the backend.
  virtual Common::List<Graphics::PixelFormat> getSupportedFormats() const override;

  // Set the size of the video bitmap.
  // Typically, 320x200
  virtual void initSize(uint w, uint h, const Graphics::PixelFormat *format) override;
  virtual int16 getHeight() override { return _screen_h; }
  virtual int16 getWidth() override { return _screen_w; }

  // Draw a bitmap to screen.
  // The screen will not be updated to reflect the new bitmap
  virtual void copyRectToScreen(const void *buf, int pitch, int x, int y, int w, int h) override;

  virtual Graphics::Surface *lockScreen() override;
  virtual void unlockScreen() override;

  // Update the dirty areas of the screen
  virtual void updateScreen() override;

  // Either show or hide the mouse cursor
  virtual bool showMouse(bool visible) override;

  // Move ("warp") the mouse cursor to the specified position.
  virtual void warpMouse(int x, int y) override;

  // Set the bitmap that's used when drawing the cursor.
  virtual void setMouseCursor(const void *buf, uint w, uint h, int hotspot_x, int hotspot_y, uint32 keycolor, bool dontScale, const Graphics::PixelFormat *format) override;

  // Replace the specified range of cursor the palette with new colors.
  virtual void setCursorPalette(const byte *colors, uint start, uint num) override;

  // Shaking is used in SCUMM. Set current shake position.
  virtual void setShakePos(int shake_x_pos, int shake_y_pos) override;

  // Get the number of milliseconds since the program was started.
  virtual uint32 getMillis(bool skipRecord = false) override;

  // Delay for a specified amount of milliseconds
  virtual void delayMillis(uint msecs) override;

  // Get the current time and date. Correspond to time()+localtime().
  virtual void getTimeAndDate(TimeDate &t) const override;

  // Get the next event.
  // Returns true if an event was retrieved.
  virtual bool pollEvent(Common::Event &event) override;

  // Quit
  virtual void quit() override;

  // Overlay
  virtual int16 getOverlayHeight() override;
  virtual int16 getOverlayWidth() override;
  virtual bool isOverlayVisible() const override { return _overlay_visible; }
  virtual void showOverlay() override;
  virtual void hideOverlay() override;
  virtual void clearOverlay() override;
  virtual void grabOverlay(void *buf, int pitch) override;
  virtual void copyRectToOverlay(const void *buf, int pitch, int x, int y, int w, int h) override;
  virtual Graphics::PixelFormat getOverlayFormat() const override { return Graphics::PixelFormat(2, 4, 4, 4, 4, 8, 4, 0, 12); }

  // Mutex handling
  virtual MutexRef createMutex() override;
  virtual void lockMutex(MutexRef mutex) override;
  virtual void unlockMutex(MutexRef mutex) override;
  virtual void deleteMutex(MutexRef mutex) override;

  // Set a window caption or any other comparable status display to the
  // given value.
  virtual void setWindowCaption(const Common::U32String &caption) override;

  // Modulatized backend
  virtual Audio::Mixer *getMixer() override { return _mixer; }

  // Extra SoftKbd support
  void mouseToSoftKbd(int x, int y, int &rx, int &ry) const;

  // Filesystem
  virtual AbstractFSNode *makeRootFileNode() const override;
  virtual AbstractFSNode *makeCurrentDirectoryFileNode() const override;
  virtual AbstractFSNode *makeFileNodePath(const Common::String &path) const override;

 private:

  Audio::MixerImpl *_mixer;
  SoftKeyboard _softkbd;

  int _ms_cur_x, _ms_cur_y, _ms_cur_w, _ms_cur_h, _ms_old_x, _ms_old_y;
  int _ms_hotspot_x, _ms_hotspot_y, _ms_visible, _devpoll, _last_screen_refresh;
  int _current_shake_x_pos, _current_shake_y_pos, _screen_w, _screen_h;
  int _overlay_x, _overlay_y;
  unsigned char *_ms_buf;
  uint32 _ms_keycolor;
  bool _overlay_visible, _overlay_dirty, _screen_dirty;
  int _screen_buffer, _overlay_buffer, _mouse_buffer;
  bool _aspect_stretch, _softkbd_on, _enable_cursor_palette;
  float _overlay_fade, _xscale, _yscale, _top_offset;
  int _softkbd_motion;

  unsigned char *screen;
  unsigned short *mouse;
  unsigned short *overlay;
  void *screen_tx[NUM_BUFFERS];
  void *mouse_tx[NUM_BUFFERS];
  void *ovl_tx[NUM_BUFFERS];
  unsigned short palette[256], cursor_palette[256];

  Graphics::Surface _framebuffer;
  int _screenFormat, _mouseFormat;

  int temp_sound_buffer[RING_BUFFER_SAMPLES>>SOUND_BUFFER_SHIFT];

  uint initSound();
  void checkSound();

  void updateScreenTextures(void);
  void updateScreenPolygons(void);
  void maybeRefreshScreen(void);
  void drawMouse(int xdraw, int ydraw, int w, int h,
		 unsigned char *buf, bool visible);

  void setScaling();


  Common::SaveFileManager *createSavefileManager();

  virtual Common::SeekableReadStream *createConfigReadStream() override;
  virtual Common::WriteStream *createConfigWriteStream() override;

  virtual void logMessage(LogMessageType::Type type, const char *message) override;
  virtual Common::String getSystemLanguage() const override;

#ifdef DYNAMIC_MODULES
  class DCPlugin;

 protected:
  virtual Plugin* createPlugin(const Common::FSNode &node) const override;
  virtual bool isPluginFilename(const Common::FSNode &node) const override;
  virtual void addCustomDirectories(Common::FSList &dirs) const override;
 public:
  virtual PluginList getPlugins() override;
 private:
  const char *pluginCustomDirectory;
#endif
};


extern int handleInput(struct mapledev *pad,
		       int &mouse_x, int &mouse_y,
		       byte &shiftFlags, Interactive *inter = NULL);
extern bool selectGame(char *&, char *&, char *&, Common::Language &, Common::Platform &, class Icon &);
#ifdef DYNAMIC_MODULES
extern bool selectPluginDir(Common::String &selection, const Common::FSNode &base);
#endif
