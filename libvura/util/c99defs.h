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

/*
 * Contains hacks for getting some C99 stuff working in VC, things like
 * bool, stdint
 */

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
#define VURA_DEPRECATED
#else
#define VURA_DEPRECATED _VURA_DEPRECATED
#endif

#if defined(IS_LIBVURA)
#define VURA_EXTERNAL_DEPRECATED
#else
#define VURA_EXTERNAL_DEPRECATED VURA_DEPRECATED
#endif

#ifdef _MSC_VER
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

#ifdef _MSC_VER
#define PRAGMA_WARN_PUSH _Pragma("warning(push)")
#define PRAGMA_WARN_POP _Pragma("warning(pop)")
#define PRAGMA_WARN_DEPRECATION _Pragma("warning(disable: 4996)")
#define PRAGMA_DISABLE_DEPRECATION _Pragma("warning(disable: 4996)")
#elif defined(__clang__)
#define PRAGMA_WARN_PUSH _Pragma("clang diagnostic push")
#define PRAGMA_WARN_POP _Pragma("clang diagnostic pop")
#define PRAGMA_WARN_DEPRECATION _Pragma("clang diagnostic warning \"-Wdeprecated-declarations\"")
#define PRAGMA_DISABLE_DEPRECATION _Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"")
#elif defined(__GNUC__)
#define PRAGMA_WARN_PUSH _Pragma("GCC diagnostic push")
#define PRAGMA_WARN_POP _Pragma("GCC diagnostic pop")
#define PRAGMA_WARN_DEPRECATION _Pragma("GCC diagnostic warning \"-Wdeprecated-declarations\"")
#define PRAGMA_DISABLE_DEPRECATION _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#else
#define PRAGMA_WARN_PUSH
#define PRAGMA_WARN_POP
#define PRAGMA_WARN_DEPRECATION
#define PRAGMA_DISABLE_DEPRECATION
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
