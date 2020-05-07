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

#include "biik/decompress.h"

#include "common/array.h"
#include "common/bitstream.h"
#include "common/memstream.h"
#include "common/stream.h"

namespace Biik {

// LZW decompression based on http://ttf.mine.nu/techdocs/unpack.pl

class DecompressorLZW {
public:
	uint32 decode(Common::SeekableReadStream *stream, byte *out, uint32 outsize);

protected:
	enum {
		SET_BITS = 8,
		MAX_BITS = 12,

		CLEAR_CODE = (1 << SET_BITS),
		END_CODE = CLEAR_CODE + 1,
		FIRST_CODE = CLEAR_CODE + 2,

		MAX_TABLE = (1 << MAX_BITS)
	};

	struct Dictionary {
		uint16 prefix;
		char postfix, first_byte;
	};

	Dictionary createDictionary(uint16 prefix, char postfix, char first_byte);
};

DecompressorLZW::Dictionary DecompressorLZW::createDictionary(uint16 prefix, char postfix, char first_byte) {
	Dictionary dict;
	dict.prefix = prefix;
	dict.postfix = postfix;
	dict.first_byte = first_byte;
	return dict;
}

#define LZW_FIRST_BYTE(x, y) (x < FIRST_CODE ? x : y[x - FIRST_CODE].first_byte)

uint32 DecompressorLZW::decode(Common::SeekableReadStream *stream, byte *out, uint32 outsize) {
	Common::BitStream8MSB bitstream(stream, DisposeAfterUse::NO);
	Common::Array<Dictionary> dict(MAX_TABLE);
	int nbit;
	uint16 dictsize = FIRST_CODE;
	uint16 prev = CLEAR_CODE;
	uint32 size = 0;

	while (prev != END_CODE) {
		if (prev == CLEAR_CODE) {
			nbit = SET_BITS + 1;
			dictsize = FIRST_CODE;
		}

		// Get next codeword
		uint16 cw = bitstream.getBits(nbit);

		// Process the codeword cw
		if (cw != CLEAR_CODE && cw != END_CODE) {
			char newbyte;
			if (cw < dictsize) {
				newbyte = LZW_FIRST_BYTE(cw, dict);
			} else {
				assert(prev != CLEAR_CODE);
				assert(dictsize < MAX_TABLE);
				assert(cw == dictsize);
				newbyte = LZW_FIRST_BYTE(prev, dict);
			}

			if ((prev != CLEAR_CODE) && (dictsize < MAX_TABLE)) {
				dict[dictsize - FIRST_CODE] = createDictionary(prev, newbyte, LZW_FIRST_BYTE(prev, dict));
				dictsize++;

				if ((dictsize == (1 << nbit)) && (nbit < MAX_BITS)) {
					nbit++;
				}
			}

			// Write the sequence here
			char temp[1024];
			uint16 outcw = cw;
			int j = 0;
			while (outcw >= FIRST_CODE) {
				Dictionary d = dict[outcw - FIRST_CODE];
				outcw = d.prefix;
				temp[j++] = d.postfix;
			}
			temp[j++] = outcw;

			for (int i = j; i > 0; i--) {
				out[size++] = temp[i-1];
				if (size > outsize)
					return size;
			}
		}
		prev = cw;
	}

	return size;
}

Common::SeekableReadStream *decompressLZW(Common::SeekableReadStream *input, uint32 size) {
	byte *block = (byte *)malloc(size);
	if (!block)
		return nullptr;

	DecompressorLZW lzw;
	uint32 realsize = lzw.decode(input, block, size);
	if (realsize == 0) {
		free(block);
		return nullptr;
	} else if (size != realsize) {
		warning("Total size: %d, expected %d", realsize, size);
	}

	return new Common::MemoryReadStream(block, realsize, DisposeAfterUse::YES);
}

} // End of namespace Biik
