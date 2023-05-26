#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#define ASSERT_EQ(actual, expected) \
	__M_Assert_EQ(actual, expected, __func__, __FILE__, __LINE__)

template<typename T> std::string stringify(const T &t)
{
	std::stringstream ss;
	ss << t;
	std::string result = ss.str();
	return result;
}

template<typename T> std::string stringify(const std::vector<T> &t)
{
	std::stringstream ss;
	ss << "{";
	for (std::size_t i = 0; i < t.size(); i++) {
		ss << stringify(t[i]);
		if (i < t.size() - 1) {
			ss << ", ";
		}
	}
	ss << "}";
	std::string result = ss.str();
	return result;
}

template<typename T>
void __M_Assert_EQ(const T &actual, const T &expected, const char *function,
		   const char *file, int line)
{
	if (expected != actual) {
		std::cerr << "Assert failed:\t"
			  << "\n"
			  << "Expected: " << stringify(expected) << "\n"
			  << "Actual: " << stringify(actual) << "\n"
			  << "Function: " << function << "\n"
			  << "File: " << file << "\n"
			  << "Line: " << line << "\n";
		abort();
	}
}
