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

class WinHandle {
    HANDLE handle = INVALID_HANDLE_VALUE;

    inline void Clear()
    {
        if (handle && handle != INVALID_HANDLE_VALUE)
            CloseHandle(handle);
    }

public:
    inline WinHandle() {}
    inline WinHandle(HANDLE handle_) : handle(handle_) {}
    inline ~WinHandle() { Clear(); }

    inline operator HANDLE() const { return handle; }

    inline WinHandle &operator=(HANDLE handle_)
    {
        if (handle_ != handle) {
            Clear();
            handle = handle_;
        }

        return *this;
    }

    inline HANDLE *operator&() { return &handle; }

    inline bool Valid() const { return handle && handle != INVALID_HANDLE_VALUE; }
};

class WinModule {
    HMODULE handle = NULL;

    inline void Clear()
    {
        if (handle)
            FreeLibrary(handle);
    }

public:
    inline WinModule() {}
    inline WinModule(HMODULE handle_) : handle(handle_) {}
    inline ~WinModule() { Clear(); }

    inline operator HMODULE() const { return handle; }

    inline WinModule &operator=(HMODULE handle_)
    {
        if (handle_ != handle) {
            Clear();
            handle = handle_;
        }

        return *this;
    }

    inline HMODULE *operator&() { return &handle; }

    inline bool Valid() const { return handle != NULL; }
};
