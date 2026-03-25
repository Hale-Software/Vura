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

#include "../c99defs.h"

#ifdef __cplusplus
extern "C" {
#endif

    struct win_version_info {
        int major;
        int minor;
        int build;
        int revis;
    };

    static inline int win_version_compare(const struct win_version_info *dst, const struct win_version_info *src)
    {
        if (dst->major > src->major)
            return 1;
        if (dst->major == src->major) {
            if (dst->minor > src->minor)
                return 1;
            if (dst->minor == src->minor) {
                if (dst->build > src->build)
                    return 1;
                if (dst->build == src->build)
                    return 0;
            }
        }
        return -1;
    }

    EXPORT bool is_64_bit_windows(void);
    EXPORT bool is_arm64_windows(void);
    EXPORT bool get_dll_ver(const wchar_t *lib, struct win_version_info *info);
    EXPORT void get_win_ver(struct win_version_info *info);
    EXPORT uint32_t get_win_ver_int(void);

#ifdef __cplusplus
}
#endif
