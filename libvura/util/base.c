/*******************************************************************************
     Copyright (c) 2026.  by Andrew Hale <halea2196@gmail.com>

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "c99defs.h"
#include "base.h"

static int crashing = 0;
static void *log_param = NULL;
static void *crash_param = NULL;

static void def_log_handler(int log_level, const char *format, va_list args, void *param)
{
    char out[8192];
    vsnprintf(out, sizeof(out), format, args);

    switch (log_level) {
        case LOG_DEBUG:
            fprintf(stdout, "DEBUG: %s\n", out);
            fflush(stdout);
            break;

        case LOG_INFO:
            fprintf(stdout, "INFO: %s\n", out);
            fflush(stdout);
            break;

        case LOG_WARNING:
            fprintf(stdout, "WARNING: %s\n", out);
            fflush(stdout);
            break;

        case LOG_ERROR:
            fprintf(stderr, "ERROR: %s\n", out);
            fflush(stderr);
    }

    UNUSED_PARAMETER(param);
}

VURA_NORETURN static void def_crash_handler(const char *format, va_list args, void *param)
{
    vfprintf(stderr, format, args);
    exit(0);

    UNUSED_PARAMETER(param);
}

static log_handler_t log_handler = def_log_handler;
static void (*crash_handler)(const char *, va_list, void *) = def_crash_handler;

void base_get_log_handler(log_handler_t *handler, void **param)
{
    if (handler)
        *handler = log_handler;
    if (param)
        *param = log_param;
}

void base_set_log_handler(log_handler_t handler, void *param)
{
    if (!handler)
        handler = def_log_handler;

    log_param = param;
    log_handler = handler;
}

void base_set_crash_handler(void (*handler)(const char *, va_list, void *), void *param)
{
    crash_param = param;
    crash_handler = handler;
}

VURA_NORETURN void bcrash(const char *format, ...)
{
    va_list args;

    if (crashing) {
        fputs("Crashed in the crash handler", stderr);
        exit(2);
    }

    crashing = 1;
    va_start(args, format);
    crash_handler(format, args, crash_param);
    va_end(args);
    exit(0);
}

void blogva(int log_level, const char *format, va_list args)
{
    log_handler(log_level, format, args, log_param);
}

void blog(int log_level, const char *format, ...)
{
    va_list args;

    va_start(args, format);
    blogva(log_level, format, args);
    va_end(args);
}
