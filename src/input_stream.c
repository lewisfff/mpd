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

#include "input_plugin.h"
#include "config.h"

#include "input/file_input_plugin.h"

#ifdef ENABLE_ARCHIVE
#include "input/archive_input_plugin.h"
#endif

#ifdef HAVE_CURL
#include "input/curl_input_plugin.h"
#endif

#ifdef ENABLE_MMS
#include "input/mms_input_plugin.h"
#endif

#include <glib.h>
#include <assert.h>

static const struct input_plugin *const input_plugins[] = {
	&input_plugin_file,
#ifdef ENABLE_ARCHIVE
	&input_plugin_archive,
#endif
#ifdef HAVE_CURL
	&input_plugin_curl,
#endif
#ifdef ENABLE_MMS
	&input_plugin_mms,
#endif
};

static bool input_plugins_enabled[G_N_ELEMENTS(input_plugins)];

static const unsigned num_input_plugins =
	sizeof(input_plugins) / sizeof(input_plugins[0]);

void input_stream_global_init(void)
{
	for (unsigned i = 0; i < num_input_plugins; ++i)
		if (input_plugins[i]->init == NULL || input_plugins[i]->init())
			input_plugins_enabled[i] = true;
}

void input_stream_global_finish(void)
{
	for (unsigned i = 0; i < num_input_plugins; ++i)
		if (input_plugins_enabled[i] &&
		    input_plugins[i]->finish != NULL)
			input_plugins[i]->finish();
}

bool
input_stream_open(struct input_stream *is, const char *url)
{
	is->seekable = false;
	is->ready = false;
	is->offset = 0;
	is->size = -1;
	is->error = 0;
	is->mime = NULL;

	for (unsigned i = 0; i < num_input_plugins; ++i) {
		const struct input_plugin *plugin = input_plugins[i];

		if (plugin->open(is, url)) {
			assert(is->plugin != NULL);
			assert(is->plugin->open == NULL ||
			       is->plugin == plugin);
			assert(is->plugin->close != NULL);
			assert(is->plugin->read != NULL);
			assert(is->plugin->eof != NULL);
			assert(!is->seekable || is->plugin->seek != NULL);

			return true;
		}
	}

	return false;
}

bool
input_stream_seek(struct input_stream *is, off_t offset, int whence)
{
	if (is->plugin->seek == NULL)
		return false;

	return is->plugin->seek(is, offset, whence);
}

struct tag *
input_stream_tag(struct input_stream *is)
{
	assert(is != NULL);

	return is->plugin->tag != NULL
		? is->plugin->tag(is)
		: NULL;
}

size_t
input_stream_read(struct input_stream *is, void *ptr, size_t size)
{
	assert(ptr != NULL);
	assert(size > 0);

	return is->plugin->read(is, ptr, size);
}

void input_stream_close(struct input_stream *is)
{
	is->plugin->close(is);

	g_free(is->mime);
}

bool input_stream_eof(struct input_stream *is)
{
	return is->plugin->eof(is);
}

int input_stream_buffer(struct input_stream *is)
{
	if (is->plugin->buffer == NULL)
		return 0;

	return is->plugin->buffer(is);
}
