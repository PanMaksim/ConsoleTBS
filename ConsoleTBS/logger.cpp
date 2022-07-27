#include "logger.h"

#include <string>
#include <sstream>
#include <fstream>
#include <chrono>
#include <time.h>

std::ofstream main_logger{"Logs/main_log.txt", std::ios::trunc};
//std::ofstream battle_logger;

void log_in_file(const char* str_log) {
	std::time_t log_time{ std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) }; // i think there should be better way to give it to std::ctime(), what i seen it requier custom library
	char time_str[26]{};
	ctime_s(time_str, 26, &log_time);

	main_logger << time_str << "  " << str_log << '\n';
}

void log_in_file(std::string str_log) {
	std::time_t log_time{ std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) }; // i think there should be better way to give it to std::ctime(), what i seen it requier custom library
	char time_str[26]{};
	ctime_s(time_str, 26, &log_time);

	main_logger << time_str << "  " << str_log << '\n';
}

void log_in_file(const std::stringstream& sstr_log) {
	std::time_t log_time{ std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) }; // i think there should be better way to give it to std::ctime(), what i seen it requier custom library
	char time_str[26]{};
	ctime_s(time_str, 26, &log_time);

	main_logger << time_str << "  " << sstr_log.str() << '\n';
}
