#pragma once
#include "incl.h"

class mem
{
public:
	mem(void* p = nullptr) : m_ptr(p) {}
	mem(u64 p) : mem((void*)p) {};
	template <typename t>
	std::enable_if_t<std::is_pointer_v<t>, t> as() {
		return static_cast<t>(m_ptr);
	}
	template <typename t>
	std::enable_if_t<std::is_lvalue_reference_v<t>, t> as() {
		return *static_cast<std::add_pointer_t<std::remove_reference_t<t>>>(m_ptr);
	}
	template <typename t>
	std::enable_if_t<std::is_same_v<t, u64>, t> as() {
		return (t)m_ptr;
	}
public:
	mem add(u64 v) {
		return mem(as<u64>() + v);
	}
	mem sub(u64 v) {
		return mem(as<u64>() - v);
	}
	mem rip() {
		if (!as<u64>())
			return {};
		return add(as<int32_t&>()).add(4);
	}
	mem qword() {
		return add(3).rip();
	}
	mem dword() {
		return add(2).rip();
	}
	mem call() {
		return add(1).rip();
	}
	operator bool() {
		return m_ptr;
	}
public:
	void* m_ptr{};
};