#pragma once

#include <string>

namespace tbs {
	namespace global {

		void add_string_to_ui_log(const std::string* str_ptr);
		void add_string_to_ui_log(const std::string str);
	}
}