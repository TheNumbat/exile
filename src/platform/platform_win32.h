
#pragma once

platform_api   platform_build_api();

bool win32_is_debugging();

platform_error win32_create_window(platform_window* window, string title, u32 width, u32 height);
platform_error win32_destroy_window(platform_window* window);
platform_error win32_get_window_size(platform_window* window, i32* w, i32* h);

platform_error win32_swap_buffers(platform_window* window);

void		   win32_set_queue_callback(void (*enqueue)(void* queue_param, platform_event evt), void* queue_param);
void 		   win32_queue_messages(platform_window* window);
platform_error win32_wait_message();
bool 		   win32_keydown(platform_keycode key);

platform_error win32_load_library(platform_dll* dll, string file_path);
platform_error win32_free_library(platform_dll* dll);

platform_error win32_get_proc_address(void** address, platform_dll* dll, string name);
void*		   win32_get_glproc(string name);

platform_error win32_copy_file(string source, string dest, bool overwrite);
platform_error win32_get_file_attributes(platform_file_attributes* attrib, string file_path);
bool 		   win32_test_file_written(platform_file_attributes* first, platform_file_attributes* second);
platform_error win32_create_file(platform_file* file, string path, platform_file_open_op mode);
platform_error win32_close_file(platform_file* file);
platform_error win32_write_file(platform_file* file, void* mem, u32 bytes);
platform_error win32_read_file(platform_file* file, void* mem, u32 bytes);
u32			   win32_file_size(platform_file* file);
platform_error win32_get_stdout_as_file(platform_file* file);
platform_error win32_write_stdout(string str);

// if this fails, we're having big problems
void*	win32_heap_alloc(u64 bytes); // initializes memory to zero (important! the data structures assume this!)
void*	win32_heap_realloc(void* mem, u64 bytes);
void	win32_heap_free(void* mem);

// allocates a string
platform_error win32_get_bin_path(string* path);

platform_error     win32_create_thread(platform_thread* thread, i32 (*proc)(void*), void* param, bool start_suspended);
platform_error	   win32_destroy_thread(platform_thread* thread);
platform_thread_id win32_this_thread_id();
platform_error	   win32_terminate_thread(platform_thread* thread, i32 exit_code);
void 	 		   win32_exit_this_thread(i32 exit_code);
void			   win32_thread_sleep(i32 ms);
i32  			   win32_get_num_cpus();
platform_thread_join_state win32_join_thread(platform_thread* thread, i32 ms); // ms = -1 for infinite

// may want to add 
	// WaitMultipleObjects
	// InterlockedCompareExchange

platform_error win32_create_semaphore(platform_semaphore* sem, i32 initial_count, i32 max_count);
platform_error win32_destroy_semaphore(platform_semaphore* sem);
platform_error win32_signal_semaphore(platform_semaphore* sem, i32 times); 
platform_semaphore_state win32_wait_semaphore(platform_semaphore* sem, i32 ms); // ms = -1 for infinite

platform_error win32_create_mutex(platform_mutex* mut, bool aquire);
platform_error win32_destroy_mutex(platform_mutex* mut);
platform_mutex_state win32_aquire_mutex(platform_mutex* mut, i32 ms); // ms = -1 for infinite
platform_error win32_release_mutex(platform_mutex* mut);

// allocates a string
string win32_make_timef(string fmt);
// takes a preallocated string, assumes enough space
void   win32_get_timef(string fmt, string* out);
