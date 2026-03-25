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

template<typename T> class CoTaskMemPtr {
    T *ptr;

    inline void Clear()
    {
        if (ptr)
            CoTaskMemFree(ptr);
    }

public:
    inline CoTaskMemPtr() : ptr(NULL) {}
    inline CoTaskMemPtr(T *ptr_) : ptr(ptr_) {}
    inline ~CoTaskMemPtr() { Clear(); }

    inline operator T *() const { return ptr; }
    inline T *operator->() const { return ptr; }

    inline const T *Get() const { return ptr; }

    inline CoTaskMemPtr &operator=(T *val)
    {
        Clear();
        ptr = val;
        return *this;
    }

    inline T **operator&()
    {
        Clear();
        ptr = NULL;
        return &ptr;
    }
};
