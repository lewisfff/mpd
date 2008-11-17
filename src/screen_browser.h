/*
 * (c) 2004 by Kalle Wallin <kaw@linux.se>
 * Copyright (C) 2008 Max Kellermann <max@duempel.org>
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
 *
 */

#ifndef SCREEN_BROWSER_H
#define SCREEN_BROWSER_H

#include "screen.h"
#include "mpdclient.h"
#include "config.h"

#include <stdbool.h>

struct list_window;
struct list_window_state;

struct screen_browser {
	struct list_window *lw;

	mpdclient_filelist_t *filelist;
};

#ifndef NCMPC_MINI

void
sync_highlights(mpdclient_t *c, mpdclient_filelist_t *fl);

void
browser_playlist_changed(struct screen_browser *browser, mpdclient_t *c,
			 int event, gpointer data);

#endif

const char *browser_lw_callback(unsigned index, int *highlight, void *filelist);

int
browser_change_directory(struct screen_browser *browser, mpdclient_t *c,
			 filelist_entry_t *entry, const char *new_path);

bool
browser_cmd(struct screen_browser *browser,
	    struct mpdclient *c, command_t cmd);

#endif