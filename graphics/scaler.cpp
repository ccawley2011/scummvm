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

#include "graphics/scaler/intern.h"
#include "graphics/scaler/scalebit.h"
#include "common/util.h"
#include "common/system.h"
#include "common/textconsole.h"

int gBitFormat = 565;

#ifdef USE_HQ_SCALERS
// RGB-to-YUV lookup table
extern "C" {

#ifdef USE_NASM
// NOTE: if your compiler uses different mangled names, add another
//       condition here

#if !defined(_WIN32) && !defined(MACOSX) && !defined(__OS2__)
#define RGBtoYUV _RGBtoYUV
#define hqx_highbits _hqx_highbits
#define hqx_lowbits _hqx_lowbits
#define hqx_low2bits _hqx_low2bits
#define hqx_low3bits _hqx_low3bits
#define hqx_greenMask _hqx_greenMask
#define hqx_redBlueMask _hqx_redBlueMask
#define hqx_green_redBlue_Mask _hqx_green_redBlue_Mask
#endif

uint32 hqx_highbits = 0xF7DEF7DE;
uint32 hqx_lowbits = 0x0821;
uint32 hqx_low2bits = 0x0C63;
uint32 hqx_low3bits = 0x1CE7;
uint32 hqx_greenMask = 0;
uint32 hqx_redBlueMask = 0;
uint32 hqx_green_redBlue_Mask = 0;

#endif

/**
 * 16bit RGB to YUV conversion table. This table is setup by InitLUT().
 * Used by the hq scaler family.
 *
 * FIXME/TODO: The RGBtoYUV table sucks up 256 KB. This is bad.
 * In addition we never free it...
 *
 * Note: a memory lookup table is *not* necessarily faster than computing
 * these things on the fly, because of its size. The table together with
 * the code, plus the input/output GFX data, may not fit in the cache on some
 * systems, so main memory has to be accessed, which is about the worst thing
 * that can happen to code which tries to be fast...
 *
 * So we should think about ways to get this smaller / removed. Maybe we can
 * use the same technique employed by our MPEG code to reduce the size of the
 * lookup table at the cost of some additional computations?
 *
 * Of course, the above is largely a conjecture, and the actual speed
 * differences are likely to vary a lot between different architectures and
 * CPUs.
 */
uint32 *RGBtoYUV = 0;
}

void InitLUT(Graphics::PixelFormat format) {
	uint8 r, g, b;
	int Y, u, v;

	assert(format.bytesPerPixel == 2);

	// Allocate the YUV/LUT buffers on the fly if needed.
	if (RGBtoYUV == 0)
		RGBtoYUV = (uint32 *)malloc(65536 * sizeof(uint32));

	if (!RGBtoYUV)
		error("[InitLUT] Cannot allocate memory for YUV/LUT buffers");

	for (int color = 0; color < 65536; ++color) {
		format.colorToRGB(color, r, g, b);
		Y = (r + g + b) >> 2;
		u = 128 + ((r - b) >> 2);
		v = 128 + ((-r + 2 * g - b) >> 3);
		RGBtoYUV[color] = (Y << 16) | (u << 8) | v;
	}

#ifdef USE_NASM
	hqx_lowbits  = (1 << format.rShift) | (1 << format.gShift) | (1 << format.bShift),
	hqx_low2bits = (3 << format.rShift) | (3 << format.gShift) | (3 << format.bShift),
	hqx_low3bits = (7 << format.rShift) | (7 << format.gShift) | (7 << format.bShift),

	hqx_highbits = format.RGBToColor(255, 255, 255) ^ hqx_lowbits;

	// FIXME: The following code only does the right thing
	// if the color order is RGB or BGR, i.e., green is in the middle.
	hqx_greenMask   = format.RGBToColor(  0, 255,   0);
	hqx_redBlueMask = format.RGBToColor(255,   0, 255);

	hqx_green_redBlue_Mask = (hqx_greenMask << 16) | hqx_redBlueMask;
#endif
}
#endif


/** Lookup table for the DotMatrix scaler. */
uint32 g_dotmatrix[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

template<typename Pixel>
void InitDotMatrix(Graphics::PixelFormat format) {
	Pixel *dotmatrix = (Pixel *)g_dotmatrix;

	// Build dotmatrix lookup table for the DotMatrix scaler.
	dotmatrix[0] = dotmatrix[10] = format.ARGBToColor(0,  0, 63,  0);
	dotmatrix[1] = dotmatrix[11] = format.ARGBToColor(0,  0,  0, 63);
	dotmatrix[2] = dotmatrix[ 8] = format.ARGBToColor(0, 63,  0,  0);
	dotmatrix[4] = dotmatrix[ 6] =
		dotmatrix[12] = dotmatrix[14] = format.ARGBToColor(0, 63, 63, 63);
}

/** Init the scaler subsystem. */
void InitScalers(uint32 BitFormat) {
	gBitFormat = BitFormat;

	// FIXME: The pixelformat should be param to this function, not the bitformat.
	// Until then, determine the pixelformat in other ways. Unfortunately,
	// calling OSystem::getOverlayFormat() here might not be safe on all ports.
	Graphics::PixelFormat format;
	if (gBitFormat == 555) {
		format = Graphics::createPixelFormat<555>();
	} else if (gBitFormat == 565) {
		format = Graphics::createPixelFormat<565>();
	} else {
		assert(g_system);
		format = g_system->getOverlayFormat();
	}

#ifdef USE_HQ_SCALERS
	InitLUT(format);
#endif

	InitDotMatrix<uint16>(format);
}

void DestroyScalers() {
#ifdef USE_HQ_SCALERS
	free(RGBtoYUV);
	RGBtoYUV = 0;
#endif
}


/**
 * Trivial 'scaler' - in fact it doesn't do any scaling but just copies the
 * source to the destination.
 */
template<typename Pixel>
void Normal1xTemplate(const uint8 *srcPtr, uint32 srcPitch, uint8 *dstPtr, uint32 dstPitch,
							int width, int height) {
	// Spot the case when it can all be done in 1 hit
	int BytesPerPixel = sizeof(Pixel);
	if ((srcPitch == BytesPerPixel * (uint)width) && (dstPitch == BytesPerPixel * (uint)width)) {
		memcpy(dstPtr, srcPtr, BytesPerPixel * width * height);
		return;
	}
	while (height--) {
		memcpy(dstPtr, srcPtr, sizeof(uint16) * width);
		srcPtr += srcPitch;
		dstPtr += dstPitch;
	}
}

void Normal1x(const uint8 *srcPtr, uint32 srcPitch, uint8 *dstPtr, uint32 dstPitch,
							int width, int height) {
	Normal1xTemplate<uint16>(srcPtr, srcPitch, dstPtr, dstPitch, width, height);
}

#ifdef USE_SCALERS

/**
 * Trivial nearest-neighbor 2x scaler.
 */
template<typename Pixel>
void Normal2xTemplate(const uint8 *srcPtr, uint32 srcPitch, uint8 *dstPtr, uint32 dstPitch,
							int width, int height) {
	uint8 *r;
	int b = sizeof(Pixel);

	assert(IS_ALIGNED(dstPtr, 2));
	while (height--) {
		r = dstPtr;
		for (int i = 0; i < width; ++i, r += b * 2) {
			Pixel color = *(((const Pixel *)srcPtr) + i);

			*(Pixel *)(r) = color;
			*(Pixel *)(r + b) = color;
			*(Pixel *)(r + dstPitch) = color;
			*(Pixel *)(r + b + dstPitch) = color;
		}
		srcPtr += srcPitch;
		dstPtr += dstPitch << 1;
	}
}

#ifdef USE_ARM_SCALER_ASM
extern "C" void Normal2xARM(const uint8  *srcPtr,
                                  uint32  srcPitch,
                                  uint8  *dstPtr,
                                  uint32  dstPitch,
                                  int     width,
                                  int     height);

template<>
void Normal2xTemplate<uint16>(const uint8 *srcPtr, uint32 srcPitch, uint8 *dstPtr, uint32 dstPitch,
							int width, int height) {
	Normal2xARM(srcPtr, srcPitch, dstPtr, dstPitch, width, height);
}

#else

template<>
void Normal2xTemplate<uint16>(const uint8 *srcPtr, uint32 srcPitch, uint8 *dstPtr, uint32 dstPitch,
							int width, int height) {
	uint8 *r;

	assert(IS_ALIGNED(dstPtr, 4));
	while (height--) {
		r = dstPtr;
		for (int i = 0; i < width; ++i, r += 4) {
			uint32 color = *(((const uint16 *)srcPtr) + i);

			color |= color << 16;

			*(uint32 *)(r) = color;
			*(uint32 *)(r + dstPitch) = color;
		}
		srcPtr += srcPitch;
		dstPtr += dstPitch << 1;
	}
}

#endif

void Normal2x(const uint8 *srcPtr, uint32 srcPitch, uint8 *dstPtr, uint32 dstPitch,
							int width, int height) {
	Normal2xTemplate<uint16>(srcPtr, srcPitch, dstPtr, dstPitch, width, height);
}

/**
 * Trivial nearest-neighbor 3x scaler.
 */
template<typename Pixel>
void Normal3xTemplate(const uint8 *srcPtr, uint32 srcPitch, uint8 *dstPtr, uint32 dstPitch,
							int width, int height) {
	uint8 *r;
	const uint32 dstPitch2 = dstPitch * 2;
	const uint32 dstPitch3 = dstPitch * 3;
	int b = sizeof(Pixel);

	assert(IS_ALIGNED(dstPtr, 2));
	while (height--) {
		r = dstPtr;
		for (int i = 0; i < width; ++i, r += b * 3) {
			Pixel color = *(((const Pixel *)srcPtr) + i);

			*(Pixel *)(r + b * 0) = color;
			*(Pixel *)(r + b * 1) = color;
			*(Pixel *)(r + b * 2) = color;
			*(Pixel *)(r + b * 0 + dstPitch) = color;
			*(Pixel *)(r + b * 1 + dstPitch) = color;
			*(Pixel *)(r + b * 2 + dstPitch) = color;
			*(Pixel *)(r + b * 0 + dstPitch2) = color;
			*(Pixel *)(r + b * 1 + dstPitch2) = color;
			*(Pixel *)(r + b * 2 + dstPitch2) = color;
		}
		srcPtr += srcPitch;
		dstPtr += dstPitch3;
	}
}

void Normal3x(const uint8 *srcPtr, uint32 srcPitch, uint8 *dstPtr, uint32 dstPitch,
							int width, int height) {
	Normal3xTemplate<uint16>(srcPtr, srcPitch, dstPtr, dstPitch, width, height);
}

/**
 * The Scale2x filter, also known as AdvMame2x.
 * See also http://scale2x.sourceforge.net
 */
void AdvMame2x(const uint8 *srcPtr, uint32 srcPitch, uint8 *dstPtr, uint32 dstPitch,
							 int width, int height) {
	scale(2, dstPtr, dstPitch, srcPtr - srcPitch, srcPitch, 2, width, height);
}

/**
 * The Scale3x filter, also known as AdvMame3x.
 * See also http://scale2x.sourceforge.net
 */
void AdvMame3x(const uint8 *srcPtr, uint32 srcPitch, uint8 *dstPtr, uint32 dstPitch,
							 int width, int height) {
	scale(3, dstPtr, dstPitch, srcPtr - srcPitch, srcPitch, 2, width, height);
}

template<typename ColorMask>
void TV2xTemplate(const uint8 *srcPtr, uint32 srcPitch, uint8 *dstPtr, uint32 dstPitch,
					int width, int height) {
	typedef typename ColorMask::PixelType Pixel;

	const uint32 nextlineSrc = srcPitch / ColorMask::kBytesPerPixel;
	const Pixel *p = (const Pixel *)srcPtr;

	const uint32 nextlineDst = dstPitch / ColorMask::kBytesPerPixel;
	Pixel *q = (Pixel *)dstPtr;

	while (height--) {
		for (int i = 0, j = 0; i < width; ++i, j += 2) {
			Pixel p1 = *(p + i);
			Pixel pi;

			pi = (((p1 & ColorMask::kRedBlueMask) * 7) >> 3) & ColorMask::kRedBlueMask;
			pi |= (((p1 & ColorMask::kGreenMask) * 7) >> 3) & ColorMask::kGreenMask;
			pi |= p1 & ColorMask::kAlphaMask;

			uint8 r, g, b;
			Graphics::colorToRGB<ColorMask>(p1,r,g,b);
			pi = Graphics::RGBToColor<ColorMask>((r*7) / 8, (g*7) / 8, (b*7) / 8);

			*(q + j) = p1;
			*(q + j + 1) = p1;
			*(q + j + nextlineDst) = pi;
			*(q + j + nextlineDst + 1) = pi;
		}
		p += nextlineSrc;
		q += nextlineDst << 1;
	}
}

void TV2x(const uint8 *srcPtr, uint32 srcPitch, uint8 *dstPtr, uint32 dstPitch, int width, int height) {
	if (gBitFormat == 565)
		TV2xTemplate<Graphics::ColorMasks<565> >(srcPtr, srcPitch, dstPtr, dstPitch, width, height);
	else
		TV2xTemplate<Graphics::ColorMasks<555> >(srcPtr, srcPitch, dstPtr, dstPitch, width, height);
}

template <typename Pixel>
static inline Pixel DOT(const Pixel *dotmatrix, Pixel c, int j, int i) {
	return c - ((c >> 2) & dotmatrix[((j & 3) << 2) + (i & 3)]);
}


// FIXME: This scaler doesn't quite work. Either it needs to know where on the
// screen it's drawing, or the dirty rects will have to be adjusted so that
// access to the dotmatrix array are made in a consistent way. (Doing that in
// a way that also works together with aspect-ratio correction is left as an
// exercise for the reader.)

template<typename Pixel>
void DotMatrixTemplate(const uint8 *srcPtr, uint32 srcPitch, uint8 *dstPtr, uint32 dstPitch,
					int width, int height) {

	const Pixel *dotmatrix = (Pixel *)g_dotmatrix;

	const uint32 nextlineSrc = srcPitch / sizeof(Pixel);
	const Pixel *p = (const Pixel *)srcPtr;

	const uint32 nextlineDst = dstPitch / sizeof(uint16);
	Pixel *q = (Pixel *)dstPtr;

	for (int j = 0, jj = 0; j < height; ++j, jj += 2) {
		for (int i = 0, ii = 0; i < width; ++i, ii += 2) {
			Pixel c = *(p + i);
			*(q + ii) = DOT<Pixel>(dotmatrix, c, jj, ii);
			*(q + ii + 1) = DOT<Pixel>(dotmatrix, c, jj, ii + 1);
			*(q + ii + nextlineDst) = DOT<Pixel>(dotmatrix, c, jj + 1, ii);
			*(q + ii + nextlineDst + 1) = DOT<Pixel>(dotmatrix, c, jj + 1, ii + 1);
		}
		p += nextlineSrc;
		q += nextlineDst << 1;
	}
}

void DotMatrix(const uint8 *srcPtr, uint32 srcPitch, uint8 *dstPtr, uint32 dstPitch,
							int width, int height) {
	DotMatrixTemplate<uint16>(srcPtr, srcPitch, dstPtr, dstPitch, width, height);
}

#endif // #ifdef USE_SCALERS
