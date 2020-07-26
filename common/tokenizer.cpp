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

#include "common/tokenizer.h"

namespace Common {

StringTokenizer::StringTokenizer(const String &str, const String &delimiters) : _str(str), _delimiters(delimiters) {
	reset();
}

void StringTokenizer::reset() {
	_tokenBegin = _tokenEnd = 0;
}

bool StringTokenizer::empty() const {
	// Search for the next token's start (i.e. the next non-delimiter character)
	uint pos;
	return !nextDelimiter(_tokenEnd, pos);
}

String StringTokenizer::nextToken() {
	// Seek to next token's start (i.e. jump over the delimiters before next token)
	nextDelimiter(_tokenEnd, _tokenBegin);

	// Seek to the token's end (i.e. jump over the non-delimiters)
	nextNonDelimiter(_tokenBegin, _tokenEnd);

	// Return the found token
	return String(_str.c_str() + _tokenBegin, _tokenEnd - _tokenBegin);
}

bool StringTokenizer::nextDelimiter(uint start, uint &pos) const {
	pos = start;
	while (pos < _str.size()) {
		if (!_delimiters.contains(_str[pos]))
			return true;

		pos++;
	}
	return false;
}

bool StringTokenizer::nextNonDelimiter(uint start, uint &pos) const {
	pos = start;
	while (pos < _str.size()) {
		if (_delimiters.contains(_str[pos]))
			return true;

		pos++;
	}
	return false;
}

bool AdvancedStringTokenizer::nextNonDelimiter(uint start, uint &pos) const {
	char openChar = 0, closeChar = 0;
	int levels = 0;
	pos = start;
	while (pos < _str.size()) {
		if (levels > 0) {
			if (_str[pos] == closeChar)
				levels--;
			else if (_str[pos] == openChar)
				levels++;
		} else {
			size_t openPos = _openChars.find(_str[pos]);
			if (openPos != Common::String::npos) {
				openChar = _openChars[openPos];
				closeChar = _closeChars[openPos];
				levels++;
			} else if (_delimiters.contains(_str[pos])) {
				return true;
			}
		}
		pos++;
	}
	return false;
}

U32StringTokenizer::U32StringTokenizer(const U32String &str, const String &delimiters) : _str(str), _delimiters(delimiters) {
	reset();
}

void U32StringTokenizer::reset() {
	_tokenBegin = _tokenEnd = _str.begin();
}

bool U32StringTokenizer::empty() const {
	// Search for the next token's start (i.e. the next non-delimiter character)
	for (U32String::const_iterator itr = _tokenEnd; itr != _str.end(); itr++) {
		if (!_delimiters.contains(*itr)) {
			return false; // Found a token so the tokenizer is not empty
		}
	}

	// Didn't find any more tokens so the tokenizer is empty
	return true;
}

U32String U32StringTokenizer::nextToken() {
	// Skip delimiters when present at the beginning, to point to the next token
	// For example, the below loop will set _tokenBegin & _tokenEnd to 'H' for the string -> "!!--=Hello World"
	// And subsequently, skip all delimiters in the beginning of the next word.
	while (_tokenBegin != _str.end() && _delimiters.contains(*_tokenBegin)) {
		_tokenBegin++;
		_tokenEnd++;
	}

	// Loop and advance _tokenEnd until we find a delimiter at the end of a word/string
	while (_tokenBegin != _str.end() && _tokenEnd != _str.end()) {
		if (_delimiters.contains(*_tokenEnd)) {
			U32String token(_tokenBegin, _tokenEnd);
			_tokenEnd++;
			_tokenBegin = _tokenEnd;
			return token;
		}
		_tokenEnd++;
	}

	// Returning the last word if _tokenBegin iterator isn't at the end.
	if (_tokenBegin != _str.end())
		return U32String(_tokenBegin, _tokenEnd);
	else
		return U32String();
}


} // End of namespace Common
