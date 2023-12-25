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

#include "noot/decompress.h"

#include "common/array.h"
#include "common/bitstream.h"
#include "common/memstream.h"
#include "common/stream.h"

namespace Noot {

// LZW decompression based on http://ttf.mine.nu/techdocs/unpack.pl

class DecompressorLZW {
public:
	uint32 decode(const byte *in, uint32 insize, byte *out, uint32 outsize);

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

uint32 DecompressorLZW::decode(const byte *in, uint32 insize, byte *out, uint32 outsize) {
	Common::BitStreamMemoryStream *memstream = new Common::BitStreamMemoryStream(in, insize, DisposeAfterUse::NO);
	Common::BitStreamMemory8MSB bitstream(memstream, DisposeAfterUse::YES);
	Common::Array<Dictionary> dict(MAX_TABLE);
	uint16 dictsize = FIRST_CODE;
	uint16 prev = CLEAR_CODE;
	uint32 size = 0;
	int nbit;

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

			if (out) {
				for (int i = j; i > 0; i--) {
					out[size++] = temp[i-1];
					if (size >= outsize)
						return size;
				}
			} else {
				size += j;
			}
		}
		prev = cw;
	}

	return size;
}

bool decompressLZW(const byte *in, uint32 insize, byte *&out, uint32 &outsize) {
	DecompressorLZW lzw;
	if (!outsize)
		outsize = lzw.decode(in, insize, NULL, 0);

	out = new byte[outsize];
	if (!out)
		return false;

	uint32 realsize = lzw.decode(in, insize, out, outsize);
	if (realsize == 0) {
		delete[] out;
		return false;
	} else if (outsize != realsize) {
		warning("Total size: %d, expected %d", realsize, outsize);
	}

	return true;
}

bool decompressRLEXOR(const byte *in, uint32 insize, byte *out, uint32 outsize) {
	if (!out)
		return false;

	const byte *inEnd = in + insize;
	uint32 *outPtr = (uint32 *)out;

	while (in < inEnd) {
		byte code = *in++;

		if ((code & 0x80) == 0) {
			for (int i = 0; i <= code; i++) {
				uint32 xorword = READ_LE_UINT32(in);
				in += 4;

				*outPtr++ ^= xorword;
			}
		} else {
			uint32 xorword = *in++;
			code &= 0x7F;

			if (xorword == 0) {
				outPtr += code + 1;
			} else {
				xorword |= xorword << 8;
				xorword |= xorword << 16;

				for (int i = 0; i <= code; i++) {
					*outPtr++ ^= xorword;
				}
			}
		}
	}

	return true;
}

} // End of namespace Noot
