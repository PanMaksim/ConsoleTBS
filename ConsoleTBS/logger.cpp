#include "logger.h"

#include <string>
#include <sstream>
#include <fstream>
#include <time.h>

std::ofstream main_logger;
//std::ofstream battle_logger;

std::string get_current_time_as_string() {
	std::time_t current_time{ std::time(0) };
	std::tm ltm;
	localtime_s(&ltm, &current_time);
	return { std::to_string(ltm.tm_hour) + ':' + std::to_string(ltm.tm_min) + ':' + std::to_string(ltm.tm_sec) + ' ' + '-'};
}

void log_in_file(const char* str_log, bool critical_error) {
	main_logger << get_current_time_as_string();
	if (critical_error) {
		main_logger << "!!!!!!!!!!!!!!!!!!!!!CRITICAL ERROR!!!!!!!!!!!!!!!!!!!!!\n";
	}

	main_logger << "  " << str_log << std::endl; // flushed every string to actually point where error occured
}

void log_in_file(std::string str_log, bool critical_error) {
	main_logger << get_current_time_as_string();
	if (critical_error) {
		main_logger << "!!!!!!!!!!!!!!!!!!!!!CRITICAL ERROR!!!!!!!!!!!!!!!!!!!!!\n";
	}

	main_logger << "  " << str_log << std::endl;
}

void log_in_file(const std::stringstream& sstr_log, bool critical_error) {
	main_logger << get_current_time_as_string();
	if (critical_error) {
		main_logger << "!!!!!!!!!!!!!!!!!!!!!CRITICAL ERROR!!!!!!!!!!!!!!!!!!!!!\n";
	}

	main_logger << "  " << sstr_log.str() << std::endl;
}

void initialize_logger() {
	main_logger.open("Logs/main_log.txt", std::ios::trunc);
}

//void close_file() {
//	main_logger.close();
//}