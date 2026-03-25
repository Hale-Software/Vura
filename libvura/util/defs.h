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

#define UNUSED_PARAMETER(param) (void)param

#ifdef _MSC_VER
#define _VURA_DEPRECATED __declspec(deprecated)
#define VURA_NORETURN __declspec(noreturn)
#define FORCE_INLINE __forceinline
#else
#define _VURA_DEPRECATED __attribute__((deprecated))
#define VURA_NORETURN __attribute__((noreturn))
#define FORCE_INLINE inline __attribute__((always_inline))
#endif

#if defined(SWIG_TYPE_TABLE)
#define OBS_DEPRECATED
#else
#define OBS_DEPRECATED _OBS_DEPRECATED
#endif
