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

#include "biik/script.h"
#include "biik/biik.h"
#include "biik/decompress.h"

#include "common/archive.h"
#include "common/stream.h"
#include "common/tokenizer.h"

namespace Biik {

Script::Script(BiikGame *vm)
	: _vm(vm) {
	_intVars.resize(100);
	_stringVars.resize(100);
}

Script::~Script() {
}

bool Script::load(Common::SeekableReadStream *stream) {
	_script.clear();

	uint32 outSize;
	if (_vm->isBigEndian()) {
		/* uint32 unknown = */ stream->readUint32BE();
		outSize = stream->readUint32BE();
	} else {
		/* uint32 unknown = */ stream->readUint32LE();
		outSize = stream->readUint32LE();
	}

	Common::SeekableReadStream *decompressed = Biik::decompressLZW(stream, outSize);
	if (!decompressed)
		return false;

	while (!decompressed->eos()) {
		Common::String line = decompressed->readLine(false);
		if (line.empty() || line.firstChar() == '|' || !line.contains(':'))
			continue;

		size_t colon = line.find(':');
		Common::String id = line.substr(0, colon);
		Common::String commands = line.substr(colon + 1);

		_script[id] = commands;
	}

	delete decompressed;
	return true;
}

bool Script::load(Common::Archive *archive, Common::String name) {
	_script.clear();

	Common::SeekableReadStream *stream = archive->createReadStreamForMember(name);
	if (!stream)
		return false;

	return load(stream);
}

void Script::run(Common::String id) {
	Common::AdvancedStringTokenizer tokenizer(_script[id], ";", "[", "]");
	while (!tokenizer.empty()) {
		command(tokenizer.nextToken());
	}
}

const Script::CommandListEntry Script::_commandList[] = {
	{ "#", &Script::cmd_SetInt },
	{ "$", &Script::cmd_SetString },
	{ "C", &Script::cmd_Call },
	{ nullptr, nullptr }
};

void Script::command(const Common::String &command) {
	for (const CommandListEntry *cmd = &_commandList[0]; cmd->prefix; ++cmd) {
		if (strncmp(command.c_str(), cmd->prefix, strlen(cmd->prefix)) != 0)
			continue;

		if (cmd->func)
			(this->*(cmd->func))(command);
		else
			warning("Ignoring unimplemented command '%s'", command.c_str());
		return;
	}

	warning("Ignoring unrecognized command '%s'", command.c_str());
}

void Script::cmd_Call(const Common::String &command) {
	run(command.substr(1));
}

void Script::cmd_SetInt(const Common::String &command) {
	size_t equals = command.find('=');
	Common::String var = command.substr(1, equals - 1);
	Common::String val = command.substr(equals + 1);

	warning("Setting an integer variable: %s = %s", var.c_str(), val.c_str());
}

void Script::cmd_SetString(const Common::String &command) {
	size_t equals = command.find('=');
	Common::String var = command.substr(1, equals - 1);
	Common::String val = command.substr(equals + 1);

	warning("Setting a string variable: %s = %s", var.c_str(), val.c_str());
}

}
