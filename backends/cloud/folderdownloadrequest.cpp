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

#include "backends/cloud/folderdownloadrequest.h"
#include "backends/cloud/downloadrequest.h"
#include "backends/cloud/id/iddownloadrequest.h"
#include "common/debug.h"
#include "gui/downloaddialog.h"
#include "backends/networking/curl/connectionmanager.h"
#include "cloudmanager.h"

namespace Cloud {

FolderDownloadRequest::FolderDownloadRequest(Storage *storage, Storage::FileArrayCallback callback, Networking::ErrorCallback ecb, const Common::String &remoteDirectoryPath, const Common::Path &localDirectoryPath, bool recursive):
	Request(nullptr, ecb), CommandSender(nullptr), _storage(storage), _fileArrayCallback(callback),
	_remoteDirectoryPath(remoteDirectoryPath), _localDirectoryPath(localDirectoryPath), _recursive(recursive),
	_workingRequest(nullptr), _ignoreCallback(false), _totalFiles(0) {
	start();
}

FolderDownloadRequest::~FolderDownloadRequest() {
	sendCommand(GUI::kDownloadEndedCmd, 0);
	_ignoreCallback = true;
	if (_workingRequest)
		_workingRequest->finish();
	delete _fileArrayCallback;
}

void FolderDownloadRequest::start() {
	//cleanup
	_ignoreCallback = true;
	if (_workingRequest)
		_workingRequest->finish();
	_currentFile = StorageFile();
	_pendingFiles.clear();
	_failedFiles.clear();
	_ignoreCallback = false;
	_totalFiles = 0;
	_downloadedBytes = _totalBytes = _wasDownloadedBytes = _currentDownloadSpeed = 0;

	//list directory first
	_workingRequest = _storage->listDirectory(
		_remoteDirectoryPath,
		new Common::Callback<FolderDownloadRequest, const Storage::ListDirectoryResponse &>(this, &FolderDownloadRequest::directoryListedCallback),
		new Common::Callback<FolderDownloadRequest, const Networking::ErrorResponse &>(this, &FolderDownloadRequest::directoryListedErrorCallback),
		_recursive
	);
}

void FolderDownloadRequest::directoryListedCallback(const Storage::ListDirectoryResponse &response) {
	_workingRequest = nullptr;
	if (_ignoreCallback)
		return;
	_pendingFiles = response.value;

	// remove all directories
	// non-empty directories would be created by DumpFile, and empty ones are just ignored
	// also skip all hidden files (with names starting with '.') or with other names that are forbidden to sync in CloudManager
	for (Common::Array<StorageFile>::iterator i = _pendingFiles.begin(); i != _pendingFiles.end();)
		if (i->isDirectory() || !CloudMan.canSyncFilename(i->name()))
			_pendingFiles.erase(i);
		else {
			_totalBytes += i->size();
			++i;
		}

	_totalFiles = _pendingFiles.size();
	downloadNextFile();
}

void FolderDownloadRequest::directoryListedErrorCallback(const Networking::ErrorResponse &error) {
	_workingRequest = nullptr;
	if (_ignoreCallback)
		return;
	finishError(error);
}

void FolderDownloadRequest::fileDownloadedCallback(const Storage::BoolResponse &response) {
	_workingRequest = nullptr;
	if (_ignoreCallback)
		return;
	if (!response.value) _failedFiles.push_back(_currentFile);
	_downloadedBytes += _currentFile.size();
	downloadNextFile();
}

void FolderDownloadRequest::fileDownloadedErrorCallback(const Networking::ErrorResponse &error) {
	_workingRequest = nullptr;
	if (_ignoreCallback)
		return;
	fileDownloadedCallback(Storage::BoolResponse(error.request, false));
}

void FolderDownloadRequest::downloadNextFile() {
	do {
		if (_pendingFiles.empty()) {
			sendCommand(GUI::kDownloadEndedCmd, 0);
			finishDownload(_failedFiles);
			return;
		}

		_currentFile = _pendingFiles.back();
		_pendingFiles.pop_back();
	} while (_currentFile.isDirectory()); // directories are actually removed earlier, in the directoryListedCallback()

	sendCommand(GUI::kDownloadProgressCmd, (int)(getProgress() * 100));

	Common::String remotePath = _currentFile.path();
	Common::String localPathStr = remotePath;
	if (!_remoteDirectoryPath.empty()) {
		if (remotePath.hasPrefix(_remoteDirectoryPath)) {
			localPathStr.erase(0, _remoteDirectoryPath.size());
			if (_remoteDirectoryPath.lastChar() != '/' && _remoteDirectoryPath.lastChar() != '\\')
				localPathStr.erase(0, 1);
		} else {
			warning("FolderDownloadRequest: Can't process the following paths:");
			warning("remote directory: %s", _remoteDirectoryPath.c_str());
			warning("remote file under that directory: %s", remotePath.c_str());
		}
	}
	Common::Path localPath(localPathStr);
	if (!_localDirectoryPath.empty()) {
		localPath = _localDirectoryPath.join(localPath);
	}
	debug(9, "FolderDownloadRequest: %s -> %s", remotePath.c_str(), localPath.toString(Common::Path::kNativeSeparator).c_str());
	_workingRequest = _storage->downloadById(
		_currentFile.id(), localPath,
		new Common::Callback<FolderDownloadRequest, const Storage::BoolResponse &>(this, &FolderDownloadRequest::fileDownloadedCallback),
		new Common::Callback<FolderDownloadRequest, const Networking::ErrorResponse &>(this, &FolderDownloadRequest::fileDownloadedErrorCallback)
	);
}

void FolderDownloadRequest::handle() {
	uint32 microsecondsPassed = Networking::ConnectionManager::getCloudRequestsPeriodInMicroseconds();
	uint64 currentDownloadedBytes = getDownloadedBytes();
	uint64 downloadedThisPeriod = currentDownloadedBytes - _wasDownloadedBytes;
	_currentDownloadSpeed = downloadedThisPeriod * (1000000L / microsecondsPassed);
	_wasDownloadedBytes = currentDownloadedBytes;
}

void FolderDownloadRequest::restart() { start(); }

void FolderDownloadRequest::finishDownload(Common::Array<StorageFile> &files) {
	Request::finishSuccess();
	if (_fileArrayCallback)
		(*_fileArrayCallback)(Storage::FileArrayResponse(this, files));
}

double FolderDownloadRequest::getProgress() const {
	if (_totalFiles == 0 || _totalBytes == 0)
		return 0;
	return (double)getDownloadedBytes() / (double)getTotalBytesToDownload();
}

uint64 FolderDownloadRequest::getDownloadedBytes() const {
	if (_totalFiles == 0)
		return 0;

	double currentFileProgress = 0;
	DownloadRequest *downloadRequest = dynamic_cast<DownloadRequest *>(_workingRequest);
	if (downloadRequest != nullptr) {
		currentFileProgress = downloadRequest->getProgress();
	} else {
		Id::IdDownloadRequest *idDownloadRequest = dynamic_cast<Id::IdDownloadRequest *>(_workingRequest);
		if (idDownloadRequest != nullptr)
			currentFileProgress = idDownloadRequest->getProgress();
	}

	return _downloadedBytes + (uint64)(currentFileProgress * _currentFile.size());
}

uint64 FolderDownloadRequest::getTotalBytesToDownload() const {
	return _totalBytes;
}

uint64 FolderDownloadRequest::getDownloadSpeed() const {
	return _currentDownloadSpeed;
}

} // End of namespace Cloud
