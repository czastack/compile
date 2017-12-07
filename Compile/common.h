#pragma once

#define DEFINE(var) decltype(var) var

template<size_t size, typename T>
constexpr size_t lengthof(T(&arr)[size]) {
	return size;
}