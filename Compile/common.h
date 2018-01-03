#pragma once

#include <string>

#define DEFINE(var) decltype(var) var

template<size_t size, typename T>
constexpr size_t lengthof(T(&arr)[size]) {
	return size;
}

std::string& trim(std::string &s);
std::string& erase_space(std::string &s);