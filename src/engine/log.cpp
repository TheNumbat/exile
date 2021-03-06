
#include "log.h"
#include "log_html.h"
#include "util/threadstate.h"
#include "dbg.h"

log_manager* global_log = null;

log_manager log_manager::make(allocator* a) { 

	log_manager ret;

	ret.out = vector<log_out>::make(4, a);
	ret.message_queue = locking_queue<log_message>::make(32, a);
	CHECKED(create_semaphore, &ret.logging_semaphore, 0, INT32_MAX);
	global_api->create_mutex(&ret.output_mut, false);

	ret.alloc = a;
	ret.scratch = MAKE_ARENA("log scratch"_, MEGABYTES(1), a);

	return ret;
}

void log_manager::start() { 

	thread_param.out 				= &out;
	thread_param.message_queue 		= &message_queue;
	thread_param.logging_semaphore 	= &logging_semaphore;
	thread_param.running 			= true;
	thread_param.alloc 				= alloc;
	thread_param.scratch			= &scratch;
	thread_param.output_mut 		= &output_mut;

	CHECKED(create_thread, &logging_thread, &log_proc, &thread_param, false);
}

void log_manager::stop() { 

	thread_param.running = false;

	CHECKED(signal_semaphore, &logging_semaphore, 1);
	global_api->join_thread(&logging_thread, -1);
	CHECKED(destroy_thread, &logging_thread);

	thread_param.out 				= null;
	thread_param.message_queue 		= null;
	thread_param.logging_semaphore 	= null;
	thread_param.alloc 				= null;
	thread_param.scratch			= null;
	thread_param.output_mut 		= null;
}

void log_manager::destroy() { 

	if(thread_param.running) {
		stop();
	}

	FORVEC(it, out) {
		print_footer(it);
		if(it->type != log_out_type::custom) {
			it->file.destroy();
		}
	}
	// NOTE(max): just to be absolutely sure nothing slipped through
	FORQ_BEGIN(it, message_queue) {
		DESTROY_ARENA(&it->arena);
	} FORQ_END(it, message_queue);

	out.destroy();
	message_queue.destroy();
	CHECKED(destroy_semaphore, &logging_semaphore);
	global_api->destroy_mutex(&output_mut);
	DESTROY_ARENA(&scratch);
	alloc = null;
}

void log_manager::push_context(string context) { 

	LOG_DEBUG_ASSERT(this_thread_data.context_depth < MAX_CONTEXT_DEPTH);
	this_thread_data.context_stack[this_thread_data.context_depth++] = context;
}

void log_manager::pop_context() { 

	LOG_DEBUG_ASSERT(this_thread_data.context_depth > 0);
	this_thread_data.context_depth--;
}

void log_manager::add_file(platform_file file, log_level level, log_out_type type, bool flush) { 

	log_out lout;
	lout.type = type;
	lout.flush_on_message = flush;
	lout.file = buffer<platform_file,4096>::make(FPTR(write_file_wrapper), file);
	lout.level = level;
	
	print_header(&lout);
	out.push(lout);
}

void log_manager::add_stdout(log_level level, log_out_type type) { 

	log_out lout;
	lout.type = log_out_type::plaintext;
	lout.file = buffer<platform_file,4096>::make(FPTR(write_stdout_wrapper), {});
	lout.flush_on_message = true;
	lout.level = level;
	
	print_header(&lout);
	out.push(lout);
}

void log_manager::add_custom_output(log_out output) { 

	if(output.type != log_out_type::custom) {
		print_header(&output);
	}

	global_api->aquire_mutex(&output_mut);
	out.push(output);
	global_api->release_mutex(&output_mut);
}

void log_manager::rem_custom_output(log_out output) { 

	global_api->aquire_mutex(&output_mut);
	out.erase(output);
	global_api->release_mutex(&output_mut);
}

void log_manager::print_header(log_out* output) { 

	PUSH_ALLOC(alloc) {
		
		if(output->type == log_out_type::plaintext) {
			
			string header = string::makef("%-8 [%-16] [%-20] [%-5] %-2\n"_, "time"_, "thread/context"_, "file:line"_, "level"_, "message"_);

			output->file.write((void*)header.c_str, header.len - 1);
			output->file.flush();

			header.destroy();

		} else if(output->type == log_out_type::html) {

			output->file.write((void*)log_html_header.c_str, log_html_header.len - 1);
			output->file.flush();
		}

	} POP_ALLOC();
}

void log_manager::print_footer(log_out* output) { 

	if(output->type == log_out_type::html) {
			
		output->file.write((void*)log_html_footer.c_str, log_html_footer.len - 1);
		output->file.flush();
	}
}

void log_manager::msg(string msg, log_level level, code_context context) { 

	log_message lmsg;

	u32 arena_size = msg.len + this_thread_data.name.len + this_thread_data.context_depth * sizeof(string);
	
	arena_allocator arena = MAKE_ARENA("msg"_, arena_size, alloc);

	PUSH_ALLOC(&arena) {

		lmsg.msg = string::make_copy(msg);
		lmsg.publisher = context;
		lmsg.level = level;

		lmsg.context_stack = array<string>::make_memory(this_thread_data.context_depth, malloc(sizeof(string) * this_thread_data.context_depth));
		lmsg.thread_name = string::make_copy(this_thread_data.name);
		_memcpy(this_thread_data.context_stack, lmsg.context_stack.memory, sizeof(string) * this_thread_data.context_depth);
		
		lmsg.arena = arena;
		message_queue.push(lmsg);

		CHECKED(signal_semaphore, &logging_semaphore, 1);

#ifdef BLOCK_OR_EXIT_ON_ERROR
		if(level == log_level::error) {
			global_api->join_thread(&logging_thread, -1);
			global_api->debug_break();
		}
#endif
		if(level == log_level::fatal) {
			global_api->join_thread(&logging_thread, -1);
			global_api->debug_break();
		}

	} POP_ALLOC();
}

string log_message::fmt_call_stack() { 

	string cstack = string::make_cat(thread_name, "/"_);
	for(u32 j = 0; j < context_stack.capacity; j++) {
		string temp = string::make_cat_v(3, cstack, *context_stack.get(j), "/"_);
		cstack.destroy();
		cstack = temp;
	}

	return cstack;
}

string log_message::fmt_file_line() { 

	return string::makef("%:%"_, publisher.file(), publisher.line);
}

string log_message::fmt_level() { 

	string str;
	switch(level) {
	case log_level::debug:
		str = "DBG"_;
		break;
	case log_level::info:
		str = "INF"_;
		break;
	case log_level::warn:
		str = "WRN"_;
		break;
	case log_level::error:
		str = "ERR"_;
		break;
	case log_level::fatal:
		str = "FTL"_;
		break;
	case log_level::ogl:
		str = "OGL"_;
		break;
	case log_level::alloc:
		str = "ALC"_;
		break;
	case log_level::console:
		str = "CON"_;
		break;
	}

	return str;
}

string fmt_msg(log_message* msg, log_out_type type) { 

	string time = global_api->time_string();
	string cstack = msg->fmt_call_stack();
	string file_line = msg->fmt_file_line();
	string clevel = msg->fmt_level();

	string output;

	if(type == log_out_type::plaintext) {

		output = string::makef("%-8 [%-16] [%-20] [%] %\n"_, time, cstack, file_line, clevel, msg->msg);

	} else if(type == log_out_type::html) {

		output = string::makef(log_html_msg, time, cstack, file_line, clevel, msg->msg);
	}

	global_api->heap_free(time.c_str);
	cstack.destroy();
	file_line.destroy();

	return output;	
}

bool operator==(log_out l, log_out r) {

	return l.level == r.level && r.type == r.type && l.flush_on_message == r.flush_on_message && r.param == l.param;
}

i32 log_proc(void* data_) {

	log_thread_param* data = (log_thread_param*)data_;	

	begin_thread("log"_, data->alloc);
	global_dbg->profiler.register_thread(1);
		
	while(data->running) {

		global_api->wait_semaphore(data->logging_semaphore, -1);

		log_message msg;
		while(data->message_queue->try_pop(&msg)) {
			if(do_msg(data, msg)) goto stop;
		}

		global_dbg->profiler.collate();
	}

	stop:

	FORVEC(it, *data->out) {
		if(it->type != log_out_type::custom) {
			it->file.flush();
		}
	}

	global_dbg->profiler.collate();
	end_thread();

	return 0;
}

bool do_msg(log_thread_param* data, log_message msg) {

	if(msg.msg.c_str != null) {
		
		PUSH_ALLOC(data->scratch) {
			
			global_api->aquire_mutex(data->output_mut);
			FORVEC(it, *data->out) {

				if(it->level <= msg.level) {
					if(it->type == log_out_type::custom) {
						it->write(&msg, it->param);
					} else {
						string output = fmt_msg(&msg, it->type);
						it->file.write(output.c_str, output.len - 1);
						if(it->flush_on_message)
							it->file.flush();
						output.destroy();
					}
				}
			}
			global_api->release_mutex(data->output_mut);

		} POP_ALLOC();
		RESET_ARENA(data->scratch);

		DESTROY_ARENA(&msg.arena);

#ifdef BLOCK_OR_EXIT_ON_ERROR
		if(msg.level == log_level::error) {
			return true;
		}
#endif
		if(msg.level == log_level::fatal) {
			return true;
		}
	}

	return false;
}
