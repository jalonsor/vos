#ifndef PAL_MAIN_H
#define PAL_MAIN_H

/* pal
 *
 * Copyright (C) 2004, Scott Kuhl
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <glib.h>
#include <stdio.h>

#include <libintl.h>
#define _(String) gettext (String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

GDate* get_query_date(gchar* date_string, gboolean show_error);
void pal_main_reload();

/* a structure that contains all of the runtime settings */
typedef struct _Settings {
    gint cal_lines;           /* number of lines for calendar */
    gint range_days;          /* print events in the next 'range_days' days */
    gint range_neg_days;      /* print events within 'range_neg_days' days old */
    gboolean range_arg;       /* user started pal with -r */
    gchar* search_string;     /* regex to search for */
    gboolean verbose;         /* verbose output */
    GDate* query_date;        /* from argument used after -d */
    gint expunge;             /* expunge events older than 'expunge' days */
    gboolean mail;            /* --mail */
    gchar* conf_file;         /* .conf file to use */
    gboolean specified_conf_file;  /* user specified a .conf file */
    gchar* date_fmt;          /* format of date to use with -r */
    gboolean week_start_monday; /* weeks should start on monday */
    gboolean reverse_order;   /* show things in reverse order */
    gboolean cal_on_bottom;   /* show calendar last */
    gboolean no_columns;      /* don't use columns */
    gboolean hide_event_type; /* hide the type of listed events */
    gint term_cols;           /* number of columns in terminal */
    gboolean manage_events;   /* add an event to pal, interactive */
    gint event_color;         /* default event color */
    gboolean html_out;        /* html output */
    gboolean latex_out;       /* LaTeX output */
    gboolean compact_list;    /* show a compact list */
    gchar* compact_date_fmt;  /* comapct list date format */
    gchar* pal_file;          /* specified one pal file to load instead
			       * of those in pal.conf */
} Settings;

typedef struct _PalTime {
    gint hour;
    gint min;
} PalTime;

typedef struct _PalEvent {
    gchar* text;       /* description of event */
    gunichar start;    /* character used before the day in calendar */
    gunichar end;      /* character used after the day in calendar */
    gboolean hide;     /* should the event be hidden on the calendar? */
    gchar* type;       /* type of event it is (from top of calendar file) */
    gint file_num;     /* this event was in the file_num-th file loaded */
    gchar* file_name;    /* name of the file containing this event */
    gint color;        /* color to be used with this event */
    GDate* start_date; /* for recurring events, date event starts on */
    GDate* end_date;   /* for recurring events, date event ends on */
    PalTime* start_time; /* first time listed in event description */
    gboolean is_todo;  /* indicates if this event is a todo item */
    gchar* date_string; /* date string used in the file for this event */
    gboolean global;   /* TRUE if event is in a global file */
} PalEvent;

Settings* settings;
GHashTable* ht;     /* ht holds the loaded events */

#endif
