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

#include "graphics/blit.h"

#include "common/endian.h"

namespace Graphics {

namespace {

/* TODO: Move these functions into common/endian.h? */

template<int Size>
FORCEINLINE uint32 READ_PIXEL(const byte *ptr) {
	STATIC_ASSERT(Size > 0 && Size <= 4, Unrecognised_Pixel_Size);

	switch (Size) {
	case 1: return *(const uint8 *)ptr;
	case 2: return *(const uint16 *)ptr;
	case 3: return READ_UINT24(ptr);
	case 4: return *(const uint32 *)ptr;
	}
}

template<int Size>
FORCEINLINE void WRITE_PIXEL(byte *ptr, uint32 value) {
	STATIC_ASSERT(Size > 0 && Size <= 4, Unrecognised_Pixel_Size);

	switch (Size) {
	case 1: *(uint8 *)ptr = value; break;
	case 2: *(uint16 *)ptr = value; break;
	case 3: WRITE_UINT24(ptr, value); break;
	case 4: *(uint32 *)ptr = value; break;
	}
}

}

// Function to blit a rect
void copyBlit(byte *dst, const byte *src,
			   const uint dstPitch, const uint srcPitch,
			   const uint w, const uint h,
			   const uint bytesPerPixel) {
	if (dst == src)
		return;

	if (dstPitch == srcPitch && ((w * bytesPerPixel) == dstPitch)) {
		memcpy(dst, src, dstPitch * h);
	} else {
		for (uint i = 0; i < h; ++i) {
			memcpy(dst, src, w * bytesPerPixel);
			dst += dstPitch;
			src += srcPitch;
		}
	}
}

namespace {

template<int Size>
inline void keyBlitLogic(byte *dst, const byte *src, const uint w, const uint h,
						 const uint srcDelta, const uint dstDelta, const uint32 key) {
	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
			uint32 color = READ_PIXEL<Size>(src);
			if (color != key)
				WRITE_PIXEL<Size>(dst, color);

			src += Size;
			dst += Size;
		}

		src += srcDelta;
		dst += dstDelta;
	}
}

} // End of anonymous namespace

// Function to blit a rect with a transparent color key
bool keyBlit(byte *dst, const byte *src,
			   const uint dstPitch, const uint srcPitch,
			   const uint w, const uint h,
			   const uint bytesPerPixel, const uint32 key) {
	if (dst == src)
		return true;

	// Faster, but larger, to provide optimized handling for each case.
	const uint srcDelta = (srcPitch - w * bytesPerPixel);
	const uint dstDelta = (dstPitch - w * bytesPerPixel);

	if (bytesPerPixel == 1) {
		keyBlitLogic<1>(dst, src, w, h, srcDelta, dstDelta, key);
	} else if (bytesPerPixel == 2) {
		keyBlitLogic<2>(dst, src, w, h, srcDelta, dstDelta, key);
	} else if (bytesPerPixel == 3) {
		keyBlitLogic<3>(dst, src, w, h, srcDelta, dstDelta, key);
	} else if (bytesPerPixel == 4) {
		keyBlitLogic<4>(dst, src, w, h, srcDelta, dstDelta, key);
	} else {
		return false;
	}

	return true;
}

namespace {

template<int SrcSize, int DstSize, bool backward, bool hasKey>
inline void crossBlitLogic(byte *dst, const byte *src, const uint w, const uint h,
						   const PixelFormat &srcFmt, const PixelFormat &dstFmt,
						   const uint srcDelta, const uint dstDelta, const uint32 key) {
	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
			const uint32 color = READ_PIXEL<SrcSize>(src);

			if (!hasKey || color != key) {
				byte r, g, b, a;
				srcFmt.colorToARGB(color, a, r, g, b);
				WRITE_PIXEL<DstSize>(dst, dstFmt.ARGBToColor(a, r, g, b));
			}

			if (backward) {
				src -= SrcSize;
				dst -= DstSize;
			} else {
				src += SrcSize;
				dst += DstSize;
			}
		}

		if (backward) {
			src -= srcDelta;
			dst -= dstDelta;
		} else {
			src += srcDelta;
			dst += dstDelta;
		}
	}
}

template<int DstSize, bool backward, bool hasKey>
inline void crossBlitMapLogic(byte *dst, const byte *src, const uint w, const uint h,
									 const uint srcDelta, const uint dstDelta, const uint32 *map, const uint32 key) {
	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
			const byte color = *src;
			if (!hasKey || color != key) {
				WRITE_PIXEL<DstSize>(dst, map[color]);
			}

			if (backward) {
				src -= 1;
				dst -= DstSize;
			} else {
				src += 1;
				dst += DstSize;
			}
		}

		if (backward) {
			src -= srcDelta;
			dst -= dstDelta;
		} else {
			src += srcDelta;
			dst += dstDelta;
		}
	}
}

} // End of anonymous namespace

// Function to blit a rect from one color format to another
bool crossBlit(byte *dst, const byte *src,
			   const uint dstPitch, const uint srcPitch,
			   const uint w, const uint h,
			   const Graphics::PixelFormat &dstFmt, const Graphics::PixelFormat &srcFmt) {
	// Error out if conversion is impossible
	if ((srcFmt.bytesPerPixel == 1) || (dstFmt.bytesPerPixel == 1)
			 || (!srcFmt.bytesPerPixel) || (!dstFmt.bytesPerPixel))
		return false;

	// Don't perform unnecessary conversion
	if (srcFmt == dstFmt) {
		copyBlit(dst, src, dstPitch, srcPitch, w, h, dstFmt.bytesPerPixel);
		return true;
	}

	// Faster, but larger, to provide optimized handling for each case.
	const uint srcDelta = (srcPitch - w * srcFmt.bytesPerPixel);
	const uint dstDelta = (dstPitch - w * dstFmt.bytesPerPixel);

	// TODO: optimized cases for dstDelta of 0
	if (dstFmt.bytesPerPixel == 2) {
		if (srcFmt.bytesPerPixel == 2) {
			crossBlitLogic<2, 2, false, false>(dst, src, w, h, srcFmt, dstFmt, srcDelta, dstDelta, 0);
		} else if (srcFmt.bytesPerPixel == 3) {
			crossBlitLogic<3, 2, false, false>(dst, src, w, h, srcFmt, dstFmt, srcDelta, dstDelta, 0);
		} else {
			crossBlitLogic<4, 2, false, false>(dst, src, w, h, srcFmt, dstFmt, srcDelta, dstDelta, 0);
		}
	} else if (dstFmt.bytesPerPixel == 3) {
		if (srcFmt.bytesPerPixel == 2) {
			// We need to blit the surface from bottom right to top left here.
			// This is neeeded, because when we convert to the same memory
			// buffer copying the surface from top left to bottom right would
			// overwrite the source, since we have more bits per destination
			// color than per source color.
			dst += h * dstPitch - dstDelta - dstFmt.bytesPerPixel;
			src += h * srcPitch - srcDelta - srcFmt.bytesPerPixel;
			crossBlitLogic<2, 3, true, false>(dst, src, w, h, srcFmt, dstFmt, srcDelta, dstDelta, 0);
		} else if (srcFmt.bytesPerPixel == 3) {
			crossBlitLogic<3, 3, false, false>(dst, src, w, h, srcFmt, dstFmt, srcDelta, dstDelta, 0);
		} else {
			crossBlitLogic<4, 3, false, false>(dst, src, w, h, srcFmt, dstFmt, srcDelta, dstDelta, 0);
		}
	} else if (dstFmt.bytesPerPixel == 4) {
		if (srcFmt.bytesPerPixel == 2) {
			// We need to blit the surface from bottom right to top left here.
			// This is neeeded, because when we convert to the same memory
			// buffer copying the surface from top left to bottom right would
			// overwrite the source, since we have more bits per destination
			// color than per source color.
			dst += h * dstPitch - dstDelta - dstFmt.bytesPerPixel;
			src += h * srcPitch - srcDelta - srcFmt.bytesPerPixel;
			crossBlitLogic<2, 4, true, false>(dst, src, w, h, srcFmt, dstFmt, srcDelta, dstDelta, 0);
		} else if (srcFmt.bytesPerPixel == 3) {
			// We need to blit the surface from bottom right to top left here.
			// This is neeeded, because when we convert to the same memory
			// buffer copying the surface from top left to bottom right would
			// overwrite the source, since we have more bits per destination
			// color than per source color.
			dst += h * dstPitch - dstDelta - dstFmt.bytesPerPixel;
			src += h * srcPitch - srcDelta - srcFmt.bytesPerPixel;
			crossBlitLogic<3, 4, true, false>(dst, src, w, h, srcFmt, dstFmt, srcDelta, dstDelta, 0);
		} else {
			crossBlitLogic<4, 4, false, false>(dst, src, w, h, srcFmt, dstFmt, srcDelta, dstDelta, 0);
		}
	} else {
		return false;
	}
	return true;
}

// Function to blit a rect from one color format to another with a transparent color key
bool crossKeyBlit(byte *dst, const byte *src,
			   const uint dstPitch, const uint srcPitch,
			   const uint w, const uint h,
			   const Graphics::PixelFormat &dstFmt, const Graphics::PixelFormat &srcFmt, const uint32 key) {
	// Error out if conversion is impossible
	if ((srcFmt.bytesPerPixel == 1) || (dstFmt.bytesPerPixel == 1)
			 || (!srcFmt.bytesPerPixel) || (!dstFmt.bytesPerPixel))
		return false;

	// Don't perform unnecessary conversion
	if (srcFmt == dstFmt) {
		keyBlit(dst, src, dstPitch, srcPitch, w, h, dstFmt.bytesPerPixel, key);
		return true;
	}

	// Faster, but larger, to provide optimized handling for each case.
	const uint srcDelta = (srcPitch - w * srcFmt.bytesPerPixel);
	const uint dstDelta = (dstPitch - w * dstFmt.bytesPerPixel);

	// TODO: optimized cases for dstDelta of 0
	if (dstFmt.bytesPerPixel == 2) {
		if (srcFmt.bytesPerPixel == 2) {
			crossBlitLogic<2, 2, false, true>(dst, src, w, h, srcFmt, dstFmt, srcDelta, dstDelta, key);
		} else if (srcFmt.bytesPerPixel == 3) {
			crossBlitLogic<3, 2, false, true>(dst, src, w, h, srcFmt, dstFmt, srcDelta, dstDelta, key);
		} else {
			crossBlitLogic<4, 2, false, true>(dst, src, w, h, srcFmt, dstFmt, srcDelta, dstDelta, key);
		}
	} else if (dstFmt.bytesPerPixel == 3) {
		if (srcFmt.bytesPerPixel == 2) {
			// We need to blit the surface from bottom right to top left here.
			// This is neeeded, because when we convert to the same memory
			// buffer copying the surface from top left to bottom right would
			// overwrite the source, since we have more bits per destination
			// color than per source color.
			dst += h * dstPitch - dstDelta - dstFmt.bytesPerPixel;
			src += h * srcPitch - srcDelta - srcFmt.bytesPerPixel;
			crossBlitLogic<2, 3, true, true>(dst, src, w, h, srcFmt, dstFmt, srcDelta, dstDelta, key);
		} else if (srcFmt.bytesPerPixel == 3) {
			crossBlitLogic<3, 3, false, true>(dst, src, w, h, srcFmt, dstFmt, srcDelta, dstDelta, key);
		} else {
			crossBlitLogic<4, 3, false, true>(dst, src, w, h, srcFmt, dstFmt, srcDelta, dstDelta, key);
		}
	} else if (dstFmt.bytesPerPixel == 4) {
		if (srcFmt.bytesPerPixel == 2) {
			// We need to blit the surface from bottom right to top left here.
			// This is neeeded, because when we convert to the same memory
			// buffer copying the surface from top left to bottom right would
			// overwrite the source, since we have more bits per destination
			// color than per source color.
			dst += h * dstPitch - dstDelta - dstFmt.bytesPerPixel;
			src += h * srcPitch - srcDelta - srcFmt.bytesPerPixel;
			crossBlitLogic<2, 4, true, true>(dst, src, w, h, srcFmt, dstFmt, srcDelta, dstDelta, key);
		} else if (srcFmt.bytesPerPixel == 3) {
			// We need to blit the surface from bottom right to top left here.
			// This is neeeded, because when we convert to the same memory
			// buffer copying the surface from top left to bottom right would
			// overwrite the source, since we have more bits per destination
			// color than per source color.
			dst += h * dstPitch - dstDelta - dstFmt.bytesPerPixel;
			src += h * srcPitch - srcDelta - srcFmt.bytesPerPixel;
			crossBlitLogic<3, 4, true, true>(dst, src, w, h, srcFmt, dstFmt, srcDelta, dstDelta, key);
		} else {
			crossBlitLogic<4, 4, false, true>(dst, src, w, h, srcFmt, dstFmt, srcDelta, dstDelta, key);
		}
	} else {
		return false;
	}
	return true;
}

// Function to blit a rect from one color format to another using a map
bool crossBlitMap(byte *dst, const byte *src,
			   const uint dstPitch, const uint srcPitch,
			   const uint w, const uint h,
			   const uint bytesPerPixel, const uint32 *map) {
	// Error out if conversion is impossible
	if (!bytesPerPixel)
		return false;

	// Faster, but larger, to provide optimized handling for each case.
	const uint srcDelta = (srcPitch - w);
	const uint dstDelta = (dstPitch - w * bytesPerPixel);

	if (bytesPerPixel == 1) {
		crossBlitMapLogic<1, false, false>(dst, src, w, h, srcDelta, dstDelta, map, 0);
	} else if (bytesPerPixel == 2) {
		// We need to blit the surface from bottom right to top left here.
		// This is neeeded, because when we convert to the same memory
		// buffer copying the surface from top left to bottom right would
		// overwrite the source, since we have more bits per destination
		// color than per source color.
		dst += h * dstPitch - dstDelta - bytesPerPixel;
		src += h * srcPitch - srcDelta - 1;
		crossBlitMapLogic<2, true, false>(dst, src, w, h, srcDelta, dstDelta, map, 0);
	} else if (bytesPerPixel == 3) {
		// We need to blit the surface from bottom right to top left here.
		// This is neeeded, because when we convert to the same memory
		// buffer copying the surface from top left to bottom right would
		// overwrite the source, since we have more bits per destination
		// color than per source color.
		dst += h * dstPitch - dstDelta - bytesPerPixel;
		src += h * srcPitch - srcDelta - 1;
		crossBlitMapLogic<3, true, false>(dst, src, w, h, srcDelta, dstDelta, map, 0);
	} else if (bytesPerPixel == 4) {
		// We need to blit the surface from bottom right to top left here.
		// This is neeeded, because when we convert to the same memory
		// buffer copying the surface from top left to bottom right would
		// overwrite the source, since we have more bits per destination
		// color than per source color.
		dst += h * dstPitch - dstDelta - bytesPerPixel;
		src += h * srcPitch - srcDelta - 1;
		crossBlitMapLogic<4, true, false>(dst, src, w, h, srcDelta, dstDelta, map, 0);
	} else {
		return false;
	}
	return true;
}

// Function to blit a rect from one color format to another using a map with a transparent color key
bool crossKeyBlitMap(byte *dst, const byte *src,
			   const uint dstPitch, const uint srcPitch,
			   const uint w, const uint h,
			   const uint bytesPerPixel, const uint32 *map, const uint32 key) {
	// Error out if conversion is impossible
	if (!bytesPerPixel)
		return false;

	// Faster, but larger, to provide optimized handling for each case.
	const uint srcDelta = (srcPitch - w);
	const uint dstDelta = (dstPitch - w * bytesPerPixel);

	if (bytesPerPixel == 1) {
		crossBlitMapLogic<1, false, true>(dst, src, w, h, srcDelta, dstDelta, map, key);
	} else if (bytesPerPixel == 2) {
		// We need to blit the surface from bottom right to top left here.
		// This is neeeded, because when we convert to the same memory
		// buffer copying the surface from top left to bottom right would
		// overwrite the source, since we have more bits per destination
		// color than per source color.
		dst += h * dstPitch - dstDelta - bytesPerPixel;
		src += h * srcPitch - srcDelta - 1;
		crossBlitMapLogic<2, true, true>(dst, src, w, h, srcDelta, dstDelta, map, key);
	} else if (bytesPerPixel == 3) {
		// We need to blit the surface from bottom right to top left here.
		// This is neeeded, because when we convert to the same memory
		// buffer copying the surface from top left to bottom right would
		// overwrite the source, since we have more bits per destination
		// color than per source color.
		dst += h * dstPitch - dstDelta - bytesPerPixel;
		src += h * srcPitch - srcDelta - 1;
		crossBlitMapLogic<3, true, true>(dst, src, w, h, srcDelta, dstDelta, map, key);
	} else if (bytesPerPixel == 4) {
		// We need to blit the surface from bottom right to top left here.
		// This is neeeded, because when we convert to the same memory
		// buffer copying the surface from top left to bottom right would
		// overwrite the source, since we have more bits per destination
		// color than per source color.
		dst += h * dstPitch - dstDelta - bytesPerPixel;
		src += h * srcPitch - srcDelta - 1;
		crossBlitMapLogic<4, true, true>(dst, src, w, h, srcDelta, dstDelta, map, key);
	} else {
		return false;
	}
	return true;
}

} // End of namespace Graphics
