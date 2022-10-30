#pragma once

struct CaseInsensitiveComparator {
	bool operator()(const std::string& a, const std::string& b) const noexcept {
		return _stricmp(a.c_str(), b.c_str()) < 0;
	}
};

template <typename T>
using CaseInsensitiveMap = std::map<std::string, T, CaseInsensitiveComparator>;