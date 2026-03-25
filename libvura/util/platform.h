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

#include <stdio.h>
#include <wchar.h>
#include <sys/types.h>

#include "c99defs.h"

/*
 * Platform-independent functions for Accessing files, encoding, DLLs,
 * sleep, timer, and timing.
 */

#ifdef __cplusplus
extern "C" {
#endif

//EXPORT FILE *os_wfopen(const wchar_t *path, const char *mode);
//EXPORT FILE *os_fopen(const char *path, const char *mode);
//EXPORT int64_t os_fgetsize(FILE *file);

//#ifdef _WIN32
//EXPORT int os_stat(const char *file, struct stat *st);
//#else
//#define os_stat stat
//#endif

/* functions purely for convenience */
//EXPORT int64_t os_get_file_size(const char *path);
//EXPORT int64_t os_get_free_space(const char *path);

//struct os_cpu_usage_info;
//typedef struct os_cpu_usage_info os_cpu_usage_info_t;

//EXPORT os_cpu_usage_info_t *os_cpu_usage_info_start(void);
//EXPORT double os_cpu_usage_info_query(os_cpu_usage_info_t *info);
//EXPORT void os_cpu_usage_info_destroy(os_cpu_usage_info_t *info);

/**
 * Sleeps to a specific time (in nanoseconds).  Doesn't have to be super
 * accurate in terms of actual slept time because the target time is ensured.
 * Returns false if already at or past target time.
 */
//EXPORT bool os_sleepto_ns(uint64_t time_target);
//EXPORT bool os_sleepto_ns_fast(uint64_t time_target);
//EXPORT void os_sleep_ms(uint32_t duration);

//EXPORT uint64_t os_gettime_ns(void);

EXPORT char *os_get_debug_path();

//EXPORT int os_get_config_path(char *dst, size_t size, const char *name);
//EXPORT char *os_get_config_path_ptr(const char *name);

//EXPORT int os_get_program_data_path(char *dst, size_t size, const char *name);
//EXPORT char *os_get_program_data_path_ptr(const char *name);

//EXPORT char *os_get_executable_path_ptr(const char *name);

//EXPORT bool os_file_exists(const char *path);

//EXPORT size_t os_get_abs_path(const char *path, char *abspath, size_t size);
//EXPORT char *os_get_abs_path_ptr(const char *path);

//EXPORT const char *os_get_path_extension(const char *path);

//struct os_dir;
//typedef struct os_dir os_dir_t;

//struct os_dirent {
//    char d_name[256];
//    bool directory;
//};

//EXPORT os_dir_t *os_opendir(const char *path);
//EXPORT struct os_dirent *os_readdir(os_dir_t *dir);
//EXPORT void os_closedir(os_dir_t *dir);

//EXPORT int os_rmdir(const char *path);

//EXPORT char *os_getcwd(char *path, size_t size);
//EXPORT int os_chdir(const char *path);

//EXPORT uint64_t os_get_free_disk_space(const char *dir);

//#define MKDIR_EXISTS 1
//#define MKDIR_SUCCESS 0
//#define MKDIR_ERROR -1

//EXPORT int os_mkdir(const char *path);
//EXPORT int os_mkdirs(const char *path);
//EXPORT int os_rename(const char *old_path, const char *new_path);
//EXPORT int os_copyfile(const char *file_in, const char *file_out);
//EXPORT int os_safe_replace(const char *target_path, const char *from_path, const char *backup_path);

//EXPORT char *os_generate_formatted_filename(const char *extension, bool space, const char *format);

//EXPORT int os_get_physical_cores(void);
//EXPORT int os_get_logical_cores(void);

//EXPORT uint64_t os_get_sys_free_size(void);
//EXPORT uint64_t os_get_sys_total_size(void);

//struct os_proc_memory_usage {
//    uint64_t resident_size;
//    uint64_t virtual_size;
//};
//typedef struct os_proc_memory_usage os_proc_memory_usage_t;

//EXPORT bool os_get_proc_memory_usage(os_proc_memory_usage_t *usage);
//EXPORT uint64_t os_get_proc_resident_size(void);
//EXPORT uint64_t os_get_proc_virtual_size(void);

//#define UUID_STR_LENGTH 36

//EXPORT char *os_generate_uuid(void);

//EXPORT
//struct timespec *os_nstime_to_timespec(uint64_t timestamp, struct timespec *storage);

/* clang-format off */
#ifdef __APPLE__
# define ARCH_BITS 64
#else
# ifdef _WIN32
#  ifdef _WIN64
#   define ARCH_BITS 64
#  else
#   define ARCH_BITS 32
#  endif
# else
#  ifdef __LP64__
#   define ARCH_BITS 64
#  else
#   define ARCH_BITS 32
#  endif
# endif
#endif
/* clang-format on */



#ifdef __cplusplus
}
#endif
