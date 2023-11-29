#include "Utils.h"

#include <algorithm>

namespace  Utils {
	std::string Trim(const std::string& a_str) {
		std::string trimmed_str = a_str;

		trimmed_str.erase(trimmed_str.begin(), std::find_if(trimmed_str.begin(), trimmed_str.end(), [](unsigned char ch) {
			return !std::isspace(ch);
		}));

		trimmed_str.erase(std::find_if(trimmed_str.rbegin(), trimmed_str.rend(), [](unsigned char ch) {
			return !std::isspace(ch);
		}).base(), trimmed_str.end());

		return trimmed_str;
	}

	std::string ToLower(const std::string& str) {
		std::string lowercase_str = str;
		std::transform(lowercase_str.begin(), lowercase_str.end(), lowercase_str.begin(), [](unsigned char c) {
			return std::tolower(c);
		});
		return lowercase_str;
	}

	std::string NormalizePath(const std::string& a_path) {
		std::string retval = a_path;
		std::replace(retval.begin(), retval.end(), '/', '\\');
		return retval;
	}
}