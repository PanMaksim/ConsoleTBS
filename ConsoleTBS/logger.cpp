#include "logger.h"

#include <string>
#include <sstream>
#include <fstream>
#include <chrono>
#include <time.h>

std::ofstream main_logger{"Logs/main_log.txt", std::ios::trunc};
//std::ofstream battle_logger;

void log_in_file(const char* str_log, bool critical_error) {
	std::time_t log_time{ std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) }; // i think there should be better way to give it to std::ctime(), what i seen it requier custom library
	char time_str[26]{};
	ctime_s(time_str, 26, &log_time);

	main_logger << time_str;
	if (critical_error) {
		main_logger << "!!!!!!!!!!!!!!!!!!!!!CRITICAL ERROR!!!!!!!!!!!!!!!!!!!!!\n";
	}

	main_logger << "  " << str_log << std::endl; // flushed every string to actually point where error occured
}

void log_in_file(std::string str_log, bool critical_error) {
	std::time_t log_time{ std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) }; // i think there should be better way to give it to std::ctime(), what i seen it requier custom library
	char time_str[26]{};
	ctime_s(time_str, 26, &log_time);

	main_logger << time_str;
	if (critical_error) {
		main_logger << "!!!!!!!!!!!!!!!!!!!!!CRITICAL ERROR!!!!!!!!!!!!!!!!!!!!!\n";
	}

	main_logger << "  " << str_log << std::endl;
}

void log_in_file(const std::stringstream& sstr_log, bool critical_error) {
	std::time_t log_time{ std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) }; // i think there should be better way to give it to std::ctime(), what i seen it requier custom library
	char time_str[26]{};
	ctime_s(time_str, 26, &log_time);

	main_logger << time_str;
	if (critical_error) {
		main_logger << "!!!!!!!!!!!!!!!!!!!!!CRITICAL ERROR!!!!!!!!!!!!!!!!!!!!!\n";
	}

	main_logger << "  " << sstr_log.str() << std::endl;
}

//void close_file() {
//	main_logger.close();
//}