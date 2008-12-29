/* the Music Player Daemon (MPD)
 * Copyright (C) 2003-2007 by Warren Dukes (warren.dukes@gmail.com)
 * This project's homepage is: http://www.musicpd.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "normalize.h"
#include "compress.h"
#include "conf.h"
#include "audio_format.h"

#define DEFAULT_VOLUME_NORMALIZATION 0

int normalizationEnabled;

void initNormalization(void)
{
	normalizationEnabled = getBoolConfigParam(CONF_VOLUME_NORMALIZATION, 1);
	if (normalizationEnabled == CONF_BOOL_UNSET)
		normalizationEnabled = DEFAULT_VOLUME_NORMALIZATION;

	if (normalizationEnabled)
		CompressCfg(0, ANTICLIP, TARGET, GAINMAX, GAINSMOOTH, BUCKETS);
}

void finishNormalization(void)
{
	if (normalizationEnabled) CompressFree();
}

void normalizeData(char *buffer, int bufferSize,
		   const struct audio_format *format)
{
	if ((format->bits != 16) || (format->channels != 2)) return;

	CompressDo(buffer, bufferSize);
}