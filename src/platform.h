
#pragma once

#include "platform_api.h"

platform_error platform_create_window(platform_window* window, const char* title, u32 width, u32 height);
platform_error platform_destroy_window(platform_window* window);

platform_error platform_swap_buffers(platform_window* window);

bool 		   platform_process_messages(platform_window* window);
platform_error platform_wait_message();

platform_error platform_load_library(platform_dll* dll, const char* file_path);
platform_error platform_free_library(platform_dll* dll);

platform_api   platform_build_api();

platform_error platform_get_proc_address(void** address, platform_dll* dll, const char* name);

platform_error platform_copy_file(const char* source, const char* dest, bool overwrite);
platform_error platform_get_file_attributes(platform_file_attributes* attrib, const char* file_path);
bool 		   platform_test_file_written(platform_file_attributes* first, platform_file_attributes* second);

// if this fails, we're having big problems
void*		   platform_heap_alloc(u64 bytes);
void	  	   platform_heap_free(void* mem);

#ifdef _WIN32

#include "platform_win32.h"

#else

#error "Unsupported platform."

#endif

