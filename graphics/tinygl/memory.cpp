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

/*
 * This file is based on, or a modified version of code from TinyGL (C) 1997-2022 Fabrice Bellard,
 * which is licensed under the MIT license (see LICENSE).
 * It also has modifications by the ResidualVM-team, which are covered under the GPLv2 (or later).
 */

// Memory allocator for TinyGL

#include "graphics/tinygl/zgl.h"
#include "common/util.h"

namespace TinyGL {

// modify these functions so that they suit your needs

static size_t max = 0;
static size_t total = 0;

void log_size(const char *prefix, int size) {
#if 1
	Common::String sizeUnits, totalUnits, maxUnits;
	Common::String sizeStr = Common::getHumanReadableBytes(size, sizeUnits);
	Common::String totalStr = Common::getHumanReadableBytes(total, totalUnits);
	Common::String maxStr = Common::getHumanReadableBytes(max, maxUnits);
	warning("%s %s %s, %s %s total, %s %s max",
	        prefix, sizeStr.c_str(), sizeUnits.c_str(),
	        totalStr.c_str(), totalUnits.c_str(),
	        maxStr.c_str(), maxUnits.c_str());
#endif
}

void gl_free(void *p) {
	if (!p)
		return;

	int *buf = ((int *)p) - 1;
	int size = *buf;
	free(buf);

	total -= size;
	log_size("Freed", size);
}

void *gl_malloc(int size) {
	int *buf = (int *)malloc(size + sizeof(int));
	if (!buf)
		return NULL;
	*buf = size;

	total += size;
	if (max < total)
		max = total;

	log_size("Allocated", size);

	return buf + 1;
}

void *gl_zalloc(int size) {
	int *buf = (int *)calloc(1, size + sizeof(int));
	if (!buf)
		return NULL;
	*buf = size;

	total += size;
	if (max < total)
		max = total;

	log_size("Allocated", size);

	return buf + 1;
}

void *gl_realloc(void *p, int size) {
	int *buf = ((int *)p) - 1;
	int old_size = *buf;

	int *new_buf = (int *)realloc(buf, size + sizeof(int));
	if (!new_buf)
		return NULL;
	*new_buf = size;

	total += size - old_size;
	if (max < total)
		max = total;

	if (size > old_size)
		log_size("Increased allocation by", size - old_size);
	else
		log_size("Decreased allocation by", old_size - size);

	return new_buf + 1;
}

} // end of namespace TinyGL
