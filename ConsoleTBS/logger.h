#pragma once

#include <string>

//#define debug_log

void log_in_file(const char* str_log, bool critical_error = false);
void log_in_file(std::string str_log, bool critical_error = false);
void log_in_file(const std::stringstream& sstr_log, bool critical_error = false);

void initialize_logger();
//void close_file();