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

#pragma once

#include <stdarg.h>

#include "defs.h"

/*
 * Logging and Crash Related Stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

enum {
    LOG_ERROR = 100,
    LOG_WARNING = 200,
    LOG_INFO = 300,
    LOG_DEBUG = 400
};

typedef void (*log_handler_t)(int lvl, const char *msg, va_list args, void *p);

EXPORT void base_get_log_handler(log_handler_t *handler, void **param);
EXPORT void base_set_log_handler(log_handler_t handler, void *param);

EXPORT void base_set_crash_handler(void (*handler)(const char *, va_list, void *), void *param);

EXPORT void blogva(int log_level, const char *format, va_list args);

EXPORT void blog(int log_level, const char *format, ...);

EXPORT void bcrash(const char *format, ...);

#ifdef __cplusplus
}
#endif
