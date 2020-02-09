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

#ifndef COMMON_LOG_H
#define COMMON_LOG_H

#include "common/scummsys.h"

namespace Common {
class WriteStream;

class Log {
public:
	virtual ~Log() {}

	/**
	 * Opens a new log file.
	 *
	 * The previous log, which was handled by this logger, will be closed
	 * before the new stream is associated.
	 *
	 * The current implemention will always call flush after data is written
	 * to the log file. It might thus be wise to pass an unbuffered write
	 * stream here to avoid unnecessary overhead.
	 * @see Common::WriteStream::flush
	 *
	 * Calling open with stream being 0 is valid and will result in the same
	 * behavior as calling close, but it may have additional overhead.
	 * @see close
	 *
	 * This function will output information about the ScummVM version and
	 * the features built into ScummVM automatically. It will also add a short
	 * notice to indicate that the log was opened successfully.
	 *
	 * @param stream Stream where to output the log contents.
	 *               Note that the stream will be deleted by the logger.
	 */
	virtual void open(WriteStream *stream) = 0;

	/**
	 * Closes the current log file.
	 *
	 * This function will output a line saying that the log was closed
	 * successfully. This can be used to check whether a log is incomplete
	 * because of whatever reasons.
	 */
	virtual void close() = 0;

	/**
	 * Prints a message to the log stream.
	 *
	 * This has optional support to output a timestamp on every new line.
	 * The timestamp will look like: "[YYYY-MM-DD HH:MM:SS] ".
	 * Printing of a timestamp is done by default.
	 *
	 * It might be noteworthy that this function does not append a new line
	 * to the given message.
	 *
	 * In case no stream is associated with this logger, this function will
	 * quit immediatly.
	 *
	 * @param message            The message to write.
	 * @param printTimeOnNewline Whether to print a timestamp on the start of
	 *                           a new line.
	 */
	virtual void print(const char *message, const bool printTimeOnNewline = true) = 0;
};

} // End of namespace Common

#endif
