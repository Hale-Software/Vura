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

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
//#include <mmsystem.h>
//#include <shellapi.h>
//#include <shlobj.h>
//#include <intrin.h>
//#include <psapi.h>
//#include <math.h>
//#include <rpc.h>

#include "base.h"
#include "platform.h"
//#include "windows/win-registry.h"
//#include "windows/win-version.h"

//#define MAX_SZ_LEN 256

//static uint32_t winver = 0;
//static char win_release_id[MAX_SZ_LEN] = "unavailable";

//static inline uint32_t get_winver(void)
//{
//    if (!winver) {
//        struct win_version_info ver;
//        get_win_ver(&ver);
//        winver = (ver.major << 8) | ver.minor;
//    }

//    return winver;
//}

/* returns [folder]\[name] on windows */
//static int os_get_path_internal(char *dst, size_t size, const char *name, int folder)
//{
//    wchar_t path_utf16[MAX_PATH];

//    SHGetFolderPathW(NULL, folder, NULL, SHGFP_TYPE_CURRENT, path_utf16);

//    if (os_wcs_to_utf8(path_utf16, 0, dst, size) != 0) {
//        if (!name || !*name) {
//            return (int)strlen(dst);
//        }

//        if (strcat_s(dst, size, "\\") == 0) {
//            if (strcat_s(dst, size, name) == 0) {
//                return (int)strlen(dst);
//            }
//        }
//    }

//    return -1;
//}

//static char *os_get_path_ptr_internal(const char *name, int folder)
//{
//    char *ptr;
//    wchar_t path_utf16[MAX_PATH];
//    struct dstr path;

//    SHGetFolderPathW(NULL, folder, NULL, SHGFP_TYPE_CURRENT, path_utf16);

//    os_wcs_to_utf8_ptr(path_utf16, 0, &ptr);
//    dstr_init_move_array(&path, ptr);
//    dstr_cat(&path, "\\");
//    dstr_cat(&path, name);
//    return path.array;
//}

char *os_get_debug_path()
{
    char* home = getenv("USERPROFILE");
    return home;
}

//int os_get_config_path(char *dst, size_t size, const char *name)
//{
//    return os_get_path_internal(dst, size, name, CSIDL_APPDATA);
//}

//char *os_get_config_path_ptr(const char *name)
//{
//    return os_get_path_ptr_internal(name, CSIDL_APPDATA);
//}

//int os_get_program_data_path(char *dst, size_t size, const char *name)
//{
//    return os_get_path_internal(dst, size, name, CSIDL_COMMON_APPDATA);
//}

//char *os_get_program_data_path_ptr(const char *name)
//{
//    return os_get_path_ptr_internal(name, CSIDL_COMMON_APPDATA);
//}

//char *os_get_executable_path_ptr(const char *name)
//{
//    char *ptr;
//    char *slash;
//    wchar_t path_utf16[MAX_PATH];
//    struct dstr path;

//    GetModuleFileNameW(NULL, path_utf16, MAX_PATH);

//    os_wcs_to_utf8_ptr(path_utf16, 0, &ptr);
//    dstr_init_move_array(&path, ptr);
//    dstr_replace(&path, "\\", "/");
//    slash = strrchr(path.array, '/');
//    if (slash) {
//        size_t len = slash - path.array + 1;
//        dstr_resize(&path, len);
//    }

//    if (name && *name) {
//        dstr_cat(&path, name);
//    }

//    return path.array;
//}

//bool os_file_exists(const char *path)
//{
//    WIN32_FIND_DATAW wfd;
//    HANDLE hFind;
//    wchar_t *path_utf16;

//    if (!os_utf8_to_wcs_ptr(path, 0, &path_utf16))
//        return false;

//    hFind = FindFirstFileW(path_utf16, &wfd);
//    if (hFind != INVALID_HANDLE_VALUE)
//        FindClose(hFind);

//    bfree(path_utf16);
//    return hFind != INVALID_HANDLE_VALUE;
//}

//size_t os_get_abs_path(const char *path, char *abspath, size_t size)
//{
//    wchar_t wpath[MAX_PATH];
//    wchar_t wabspath[MAX_PATH];
//    size_t out_len = 0;
//    size_t len;

//    if (!abspath)
//        return 0;

//    len = os_utf8_to_wcs(path, 0, wpath, MAX_PATH);
//    if (!len)
//        return 0;

//    if (_wfullpath(wabspath, wpath, MAX_PATH) != NULL)
//        out_len = os_wcs_to_utf8(wabspath, 0, abspath, size);
//    return out_len;
//}

//char *os_get_abs_path_ptr(const char *path)
//{
//    char *ptr = bmalloc(MAX_PATH);

//    if (!os_get_abs_path(path, ptr, MAX_PATH)) {
//        bfree(ptr);
//        ptr = NULL;
//    }

//    return ptr;
//}

//struct os_dir {
//    HANDLE handle;
//    WIN32_FIND_DATA wfd;
//    bool first;
//    struct os_dirent out;
//};

//int os_rmdir(const char *path)
//{
//    wchar_t *w_path;
//    bool success;

//    os_utf8_to_wcs_ptr(path, 0, &w_path);
//    if (!w_path)
//        return -1;

//    success = !!RemoveDirectoryW(w_path);
//    bfree(w_path);

//    return success ? 0 : -1;
//}

//int os_mkdir(const char *path)
//{
//    wchar_t *path_utf16;
//    BOOL success;

//    if (!os_utf8_to_wcs_ptr(path, 0, &path_utf16))
//        return MKDIR_ERROR;

//    success = CreateDirectory(path_utf16, NULL);
//    bfree(path_utf16);

//    if (!success)
//        return (GetLastError() == ERROR_ALREADY_EXISTS) ? MKDIR_EXISTS : MKDIR_ERROR;

//    return MKDIR_SUCCESS;
//}

//int os_rename(const char *old_path, const char *new_path)
//{
//    wchar_t *old_path_utf16 = NULL;
//    wchar_t *new_path_utf16 = NULL;
//    int code = -1;

//    if (!os_utf8_to_wcs_ptr(old_path, 0, &old_path_utf16)) {
//        return -1;
//    }
//    if (!os_utf8_to_wcs_ptr(new_path, 0, &new_path_utf16)) {
//        goto error;
//    }

//    code = MoveFileExW(old_path_utf16, new_path_utf16, MOVEFILE_REPLACE_EXISTING) ? 0 : -1;

//    error:
//        bfree(old_path_utf16);
//    bfree(new_path_utf16);
//    return code;
//}

//int os_safe_replace(const char *target, const char *from, const char *backup)
//{
//    wchar_t *wtarget = NULL;
//    wchar_t *wfrom = NULL;
//    wchar_t *wbackup = NULL;
//    int code = -1;

//    if (!target || !from)
//        return -1;
//    if (!os_utf8_to_wcs_ptr(target, 0, &wtarget))
//        return -1;
//    if (!os_utf8_to_wcs_ptr(from, 0, &wfrom))
//        goto fail;
//    if (backup && !os_utf8_to_wcs_ptr(backup, 0, &wbackup))
//        goto fail;

//    if (ReplaceFileW(wtarget, wfrom, wbackup, 0, NULL, NULL)) {
//        code = 0;
//    } else if (GetLastError() == ERROR_FILE_NOT_FOUND) {
//        code = MoveFileExW(wfrom, wtarget, MOVEFILE_REPLACE_EXISTING) ? 0 : -1;
//    }

//    fail:
//        bfree(wtarget);
//    bfree(wfrom);
//    bfree(wbackup);
//    return code;
//}

//int os_copyfile(const char *file_in, const char *file_out)
//{
//    wchar_t *file_in_utf16 = NULL;
//    wchar_t *file_out_utf16 = NULL;
//    int code = -1;

//    if (!os_utf8_to_wcs_ptr(file_in, 0, &file_in_utf16)) {
//        return -1;
//    }
//    if (!os_utf8_to_wcs_ptr(file_out, 0, &file_out_utf16)) {
//        goto error;
//    }

//    code = CopyFileW(file_in_utf16, file_out_utf16, true) ? 0 : -1;

//    error:
//        bfree(file_in_utf16);
//    bfree(file_out_utf16);
//    return code;
//}

//char *os_getcwd(char *path, size_t size)
//{
//    wchar_t *path_w;
//    DWORD len;

//    len = GetCurrentDirectoryW(0, NULL);
//    if (!len)
//        return NULL;

//    path_w = bmalloc(((size_t)len + 1) * sizeof(wchar_t));
//    GetCurrentDirectoryW(len + 1, path_w);
//    os_wcs_to_utf8(path_w, (size_t)len, path, size);
//    bfree(path_w);

//    return path;
//}

//int os_chdir(const char *path)
//{
//    wchar_t *path_w = NULL;
//    size_t size;
//    int ret;

//    size = os_utf8_to_wcs_ptr(path, 0, &path_w);
//    if (!path_w)
//        return -1;

//    ret = SetCurrentDirectoryW(path_w) ? 0 : -1;
//    bfree(path_w);

//    return ret;
//}
