#pragma once
#include "mem.h"

class hmodule
{
public:
	hmodule(std::string_view name = {}) : m_name(name), m_handle(get()), m_base(m_handle) 
	{
		auto dos_header = m_base.as<IMAGE_DOS_HEADER*>();
		auto nt_header = m_base.add(dos_header->e_lfanew).as<IMAGE_NT_HEADERS*>();
		m_size = nt_header->OptionalHeader.SizeOfImage;
	}
	mem begin() {
		return m_base;
	}
	mem end() {
		return begin().add(size());
	}
	s64 size() {
		return m_size;
	}
	const bool exists() {
		return size();
	}
	HMODULE get() 
	{
		if (m_name.empty())
			return GetModuleHandleA(NULL);

		return GetModuleHandleA(m_name.data());
	}
private:
	std::string_view m_name{};
	HMODULE m_handle{};
	mem m_base{};
	s64 m_size{};
};