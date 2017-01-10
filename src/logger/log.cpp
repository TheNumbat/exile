
#include "log.hpp"
#include "../common.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <time.h>

logger glog(EMIT_LEVEL);

inline const char* LEVEL_STR(message_level lvl) {
	switch(lvl) {
		case message_info: return "INFO";
		case message_warn: return "WARN";
		case message_error: return "ERROR";
		case message_fatal: return "FATAL";
		default: INVALID_CODE_PATH; return nullptr;
	}
}
	
message::message(int indent, std::thread::id i, message_level level, std::string c, std::string m) { 
	indent_level = indent;
	lvl = level;
	context = c;
	msg = m;
	id = i;
}

logger::logger(int emit) {

	end_thread = false;
	emit_level = emit;
#ifdef LOG_FILE
	out_streams.push_back(new std::ofstream("log.txt"));
#endif
#ifdef LOG_CONSOLE
	out_streams.push_back(&std::cout);
#endif

	thread = new std::thread(&logger::logging_thread, this);
}

logger::~logger() {
	end_thread = true;
	var.notify_all();
	thread->join();
	delete thread;
#ifdef LOG_FILE
	delete out_streams[0];
#endif
}

void logger::begin_on_thread(std::string name) {
	std::unique_lock<std::mutex> lock(log_mutex);
	std::thread::id id = std::this_thread::get_id();
	thread_indent_levels.insert({id, 0});
	thread_current_context.insert({id, std::stack<std::string>()});
	thread_names.insert({id, name});
}

void logger::pushSec() {
	std::unique_lock<std::mutex> lock(log_mutex);
	std::thread::id id = std::this_thread::get_id();
	auto level_entry = thread_indent_levels.find(id);
	assert(level_entry != thread_indent_levels.end());
	level_entry->second++;
}

void logger::popSec() {
	std::unique_lock<std::mutex> lock(log_mutex);
	std::thread::id id = std::this_thread::get_id();
	auto level_entry = thread_indent_levels.find(id);
	assert(level_entry != thread_indent_levels.end());
	assert(level_entry->second > 0);
	level_entry->second--;
}

void logger::pushContext(std::string context) {
	std::unique_lock<std::mutex> lock(log_mutex);
	std::thread::id id = std::this_thread::get_id();
	auto context_entry = thread_current_context.find(id);
	assert(context_entry != thread_current_context.end());
	context_entry->second.push(context);
}

void logger::popContext() {
	std::unique_lock<std::mutex> lock(log_mutex);
	std::thread::id id = std::this_thread::get_id();
	auto context_entry = thread_current_context.find(id);
	assert(context_entry != thread_current_context.end());
	assert(context_entry->second.size() > 0);
	context_entry->second.pop();
}

void logger::info(std::string msg) {
	if(emit_level >= 3) {
		std::unique_lock<std::mutex> lock(log_mutex);
		std::thread::id id = std::this_thread::get_id();
		auto level_entry = thread_indent_levels.find(id);
		auto context_entry = thread_current_context.find(id);
		assert(level_entry != thread_indent_levels.end());
		assert(context_entry != thread_current_context.end());
		assert(context_entry->second.size() > 0);
		message_q.push(message(level_entry->second, id, message_info, context_entry->second.top(), msg));
		var.notify_all();
	}
}

void logger::warn(std::string msg) {
	if(emit_level >= 2) {
		std::unique_lock<std::mutex> lock(log_mutex);
		std::thread::id id = std::this_thread::get_id();
		auto level_entry = thread_indent_levels.find(id);
		auto context_entry = thread_current_context.find(id);
		assert(level_entry != thread_indent_levels.end());
		assert(context_entry != thread_current_context.end());
		assert(context_entry->second.size() > 0);
		message_q.push(message(level_entry->second, id, message_warn, context_entry->second.top(), msg));
		var.notify_all();
	}
}

void logger::error(std::string msg) {
	if(emit_level >= 1) {
		std::unique_lock<std::mutex> lock(log_mutex);
		std::thread::id id = std::this_thread::get_id();
		auto level_entry = thread_indent_levels.find(id);
		auto context_entry = thread_current_context.find(id);
		assert(level_entry != thread_indent_levels.end());
		assert(context_entry != thread_current_context.end());
		assert(context_entry->second.size() > 0);
		message_q.push(message(level_entry->second, id, message_error, context_entry->second.top(), msg));
		var.notify_all();
	}
}

void logger::fatal(std::string msg) {
	if(emit_level >= 0) {
		std::unique_lock<std::mutex> lock(log_mutex);
		std::thread::id id = std::this_thread::get_id();
		auto level_entry = thread_indent_levels.find(id);
		auto context_entry = thread_current_context.find(id);
		assert(level_entry != thread_indent_levels.end());
		assert(context_entry != thread_current_context.end());
		assert(context_entry->second.size() > 0);
		message_q.push(message(level_entry->second, id, message_fatal, context_entry->second.top(), msg));
		var.notify_all();
	}
}

void logger::msg(message msg) {
	std::unique_lock<std::mutex> lock(log_mutex);
	message_q.push(msg);
	var.notify_all();
}

void logger::set_emit_level(int emit) {
	emit_level = emit;
}

void logger::logging_thread() {
	LOG_BEGIN_THIS_THREAD(LOG);
	std::cout << "Log format: HH:MM:SS [thread/context/level] message" << std::endl << std::endl;
	output_message(message(0, std::this_thread::get_id(), message_info, "LOGGING", "Started logging thread."));
	while(!end_thread) {
		std::unique_lock<std::mutex> lock(log_mutex);
		var.wait(lock);
		while(message_q.size()) {
			output_message(message_q.front());
			message_q.pop();
		}
	}
	output_message(message(0, std::this_thread::get_id(), message_info, "LOGGING", "Ended logging thread."));
}

void logger::output_message(message msg) {
	for(std::ostream* out : out_streams) {
		std::time_t t(0);
		time(&t);
		struct tm buf;
#ifdef _WIN32
		localtime_s(&buf, &t);
#else
		localtime_r(&t, &buf);
#endif
		*out << std::put_time(&buf, "%H:%M:%S");
		for(int i = 0; i < msg.indent_level; i++) {
			*out << "\t";
		}
		auto thread_name_entry = thread_names.find(msg.id);
		assert(thread_name_entry != thread_names.end());
		*out << " [" << thread_name_entry->second << "/" << msg.context << "/" << LEVEL_STR(msg.lvl) << "] " << msg.msg << std::endl;
	}
}

