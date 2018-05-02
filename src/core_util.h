#pragma once
#include "core.h"

namespace core {
namespace util {

inline constexpr i32 abs(i32 x) {
	return x > 0 ? x : -x;
}

inline constexpr u32 align(u32 x, u32 multiple) {
	return ((x + multiple - 1) / multiple) * multiple;
}

template <typename type_t>
inline constexpr type_t clamp(type_t x, type_t lo, type_t hi) {
	return x < lo ? lo : (x < hi ? x : hi);
}

template <typename type_t>
inline constexpr type_t max(type_t x, type_t y) {
	return (x < y) ? y : x;
}

template <typename type_t>
inline constexpr type_t min(type_t x, type_t y) {
	return (y < x) ? y : x;
}

inline u32 pow2(u32 x) {
	x -= 1;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x += 1;
	return x;
}

template <typename type_t>
inline void swap(type_t& a, type_t& b) {
	type_t temp = a;
	a = b;
	b = temp;
}

template <typename type_t>
inline void zero(type_t* ptr) {
	memset(ptr, 0, sizeof(type_t));
}

}
}
