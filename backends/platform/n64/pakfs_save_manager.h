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

#ifndef __PAKFS_SAVE_MANAGER__
#define __PAKFS_SAVE_MANAGER__

#include <common/savefile.h>
#include <common/zlib.h>

#include <pakfs.h> // N64 PakFS library

bool pakfs_deleteSaveGame(const char *filename);

class InPAKSave final : public Common::InSaveFile {
private:
	PAKFILE *fd;

	virtual uint32 read(void *buf, uint32 cnt) override;
	virtual bool skip(uint32 offset) override;
	virtual bool seek(int32 offs, int whence) override;

public:
	InPAKSave() : fd(NULL) { }

	~InPAKSave() {
		if (fd != NULL)
			pakfs_close(fd);
	}

	virtual bool eos() const override {
		return pakfs_eof(fd);
	}
	virtual void clearErr() override {
		pakfs_clearerr(fd);
	}
	virtual int32 pos() const override {
		return pakfs_tell(fd);
	}
	virtual int32 size() const override {
		return fd->size;
	}

	bool readSaveGame(const char *filename) {
		fd = pakfs_open(filename, "r");
		return (fd != NULL);
	}
};

class OutPAKSave final : public Common::WriteStream {
private:
	PAKFILE *fd;

public:
	virtual uint32 write(const void *buf, uint32 cnt) override;

	virtual int32 pos() const override {
		return pakfs_tell(fd);
	}

	OutPAKSave(const char *_filename) : fd(NULL) {
		fd = pakfs_open(_filename, "w");
	}

	~OutPAKSave() {
		if (fd != NULL) {
			finalize();
			pakfs_close(fd);
			flushCurrentPakData();
		}
	}

	virtual bool err() const override {
		if (fd)
			return (pakfs_error(fd) == 1);
		else
			return true;
	}
	virtual void clearErr() override {
		pakfs_clearerr(fd);
	}
	virtual void finalize() override {
		pakfs_flush(fd);
	}
};

class PAKSaveManager final : public Common::SaveFileManager {
public:
	virtual void updateSavefilesList(Common::StringArray &lockedFiles) {
		// this method is used to lock saves while cloud syncing
		// as there is no network on N64, this method wouldn't be used
		// thus it's not implemtented
	}

	virtual Common::InSaveFile *openRawFile(const Common::String &filename) override {
		InPAKSave *s = new InPAKSave();
		if (s->readSaveGame(filename.c_str())) {
			return s;
		} else {
			delete s;
			return NULL;
		}
	}

	virtual Common::OutSaveFile *openForSaving(const Common::String &filename, bool compress = true) override {
		OutPAKSave *s = new OutPAKSave(filename.c_str());
		if (!s->err()) {
			return new Common::OutSaveFile(compress ? Common::wrapCompressedWriteStream(s) : s);
		} else {
			delete s;
			return NULL;
		}
	}

	virtual Common::InSaveFile *openForLoading(const Common::String &filename) override {
		InPAKSave *s = new InPAKSave();
		if (s->readSaveGame(filename.c_str())) {
			return Common::wrapCompressedReadStream(s);
		} else {
			delete s;
			return NULL;
		}
	}

	virtual bool removeSavefile(const Common::String &filename) override {
		return ::pakfs_deleteSaveGame(filename.c_str());
	}

	virtual Common::StringArray listSavefiles(const Common::String &pattern) override;
};


#endif
