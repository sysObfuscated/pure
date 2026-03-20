#pragma once
#include <cstdint>
#include <string_view>

namespace rage {
	inline constexpr char jTolower(char const c) {
		return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
	}
	inline uint32_t joaat(std::string_view str, bool forceLowerCase = true) {
		uint32_t hash{};
		for (char c : str) {
			hash += forceLowerCase ? jTolower(c) : c;
			hash += (hash << 10);
			hash ^= (hash >> 6);
		}
		hash += (hash << 3);
		hash ^= (hash >> 11);
		hash += (hash << 15);
		return hash;
	}
	template <typename t = std::string_view>
	inline consteval uint32_t constexprJoaat(t str, bool const forceLowerCase = true) {
		uint32_t hash{};
		while (*str != '\0') {
			hash += forceLowerCase ? jTolower(*str) : *str, ++str;
			hash += hash << 10, hash ^= hash >> 6;
		}
		hash += (hash << 3), hash ^= (hash >> 11), hash += (hash << 15);

		return hash;
	}
	template <size_t s>
	inline constexpr uint32_t constexprJoaatFromArr(char const(&data)[s]) {
		uint32_t hash{};
		for (uint64_t i{}; i != s; ++i) {
			hash += jTolower(*data);
			hash += hash << 10, hash ^= hash >> 6;
		}
		hash += (hash << 3), hash ^= (hash >> 11), hash += (hash << 15);
		return hash;
	}
}
inline consteval uint32_t operator ""_joaat(const char* str, size_t) {
	return rage::constexprJoaat(str);
}
inline consteval uint32_t operator ""_joaatNormalised(const char* str, size_t) {
	return rage::constexprJoaat(str, false);
}