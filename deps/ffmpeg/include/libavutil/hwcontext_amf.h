/*******************************************************************************
     Copyright (c) 2026 by Andrew Hale <halea2196@gmail.com>

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

/*
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */


#ifndef AVUTIL_HWCONTEXT_AMF_H
#define AVUTIL_HWCONTEXT_AMF_H

#include "pixfmt.h"
#include "hwcontext.h"
#include <AMF/core/Factory.h>
#include <AMF/core/Context.h>
#include <AMF/core/Trace.h>
#include <AMF/core/Debug.h>

/**
 * This struct is allocated as AVHWDeviceContext.hwctx
 */
typedef struct AVAMFDeviceContext {
    void *              library;
    AMFFactory         *factory;
    void               *trace_writer;

    int64_t             version; ///< version of AMF runtime
    AMFContext         *context;
    AMF_MEMORY_TYPE     memory_type;

    void (*lock)(void *lock_ctx);
    void (*unlock)(void *lock_ctx);
    void *lock_ctx;
} AVAMFDeviceContext;

enum AMF_SURFACE_FORMAT av_av_to_amf_format(enum AVPixelFormat fmt);
enum AVPixelFormat av_amf_to_av_format(enum AMF_SURFACE_FORMAT fmt);

#endif /* AVUTIL_HWCONTEXT_AMF_H */
