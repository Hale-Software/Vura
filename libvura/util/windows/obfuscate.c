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

#ifdef _MSC_VER
#pragma warning(disable : 4152) /* casting func ptr to void */
#endif

#include <stdbool.h>
#include <windows.h>
#include "obfuscate.h"

#define LOWER_HALFBYTE(x) ((x) & 0xF)
#define UPPER_HALFBYTE(x) (((x) >> 4) & 0xF)

static void deobfuscate_str(char *str, uint64_t val)
{
    uint8_t *dec_val = (uint8_t *)&val;
    int i = 0;

    while (*str != 0) {
        int pos = i / 2;
        bool bottom = (i % 2) == 0;
        uint8_t *ch = (uint8_t *)str;
        uint8_t xor = bottom ? LOWER_HALFBYTE(dec_val[pos]) : UPPER_HALFBYTE(dec_val[pos]);

        *ch ^= xor;

        if (++i == sizeof(uint64_t) * 2)
            i = 0;

        str++;
    }
}

void *ms_get_obfuscated_func(HMODULE module, const char *str, uint64_t val)
{
    char new_name[128];
    strcpy(new_name, str);
    deobfuscate_str(new_name, val);
    return GetProcAddress(module, new_name);
}
