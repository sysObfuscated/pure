#pragma once
#include "base/module.h"

namespace pure::memory
{
	// for wildcard support
	using u8_byte = std::optional<u8>;

	//simple shellcode (mov rax, jmp rax)
	constexpr inline u8 g_shellcode[12]{ 0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xE0 };
	constexpr inline u64 g_shellcode_size = sizeof(g_shellcode);

	// temp change memory protection
	inline u32 protect_memory_region(void* address, u64 size, std::function<void()> cb)
	{
		u32 old_prot{};

		VirtualProtect(reinterpret_cast<void*>(address), size, PAGE_EXECUTE_READWRITE, reinterpret_cast<DWORD*>(&old_prot));
		cb();
		VirtualProtect(reinterpret_cast<void*>(address), size, old_prot, reinterpret_cast<DWORD*>(&old_prot));

		return old_prot;
	}

	// convert a single hex char into a value
	inline u8_byte char_to_hex(const char c)
	{
		if (c >= 'a' && c <= 'f')
			return static_cast<u8>(static_cast<s32>(c) - 87);
		if (c >= 'A' && c <= 'F')
			return static_cast<u8>(static_cast<s32>(c) - 55);
		if (c >= '0' && c <= '9')
			return static_cast<u8>(static_cast<s32>(c) - 48);

		return {};
	}

	// converts pattern string into vector of bytes + wildcards
	inline std::vector<u8_byte> create_bytes_from_string(const std::string& ptr)
	{
		std::vector<u8_byte> bytes{};

		for (s64 i{}; i != ptr.size() - 1; ++i)
		{
			// skip spaces
			if (ptr[i] == ' ')
				continue;

			// normal
			if (ptr[i] != '?')
			{
				if (auto c{ char_to_hex(ptr[i]) })
				{
					if (auto c2{ char_to_hex(ptr[i + 1]) })
					{
						bytes.emplace_back(static_cast<u8>((*c * 0x10) + *c2));
					}
				}
			}
			else
				bytes.push_back(std::nullopt);
		}

		return bytes;
	}

	// simple pattern scanner
	inline mem scan(std::string pattern, hmodule module = {})
	{
		const std::vector<u8_byte> sig = create_bytes_from_string(pattern);
		const u64 length = sig.size();

		// setup shift table
		u64 max_shift = length, max = length - 1;
		u64 wildcard = static_cast<size_t>(-1);
		u64 shift_table[UINT8_MAX + 1]{};

		// find first wildcard from the end
		for (int i{ static_cast<int>(max - 1) }; i >= 0; --i)
		{
			if (!sig[i])
			{
				max_shift = max - i;
				wildcard = i;
				break;
			}
		}

		// default = max_shift
		std::fill(std::begin(shift_table), std::end(shift_table), max_shift);

		// fill shift table based on pattern
		for (u64 i{ wildcard + 1 }; i != max; ++i)
			shift_table[*sig[i]] = max - i;

		// scan module memory
		for (u64 idx{}; idx <= module.size() - length;)
		{
			// compare pattern backwards

			for (s64 sig_idx{ (s64)max }; sig_idx >= 0; --sig_idx)
			{
				// mismatch
				if (sig[sig_idx] && *module.begin().add(idx + sig_idx).as<u8*>() != *sig[sig_idx])
				{
					idx += shift_table[*module.begin().add(idx + max).as<u8*>()];
					break;
				}
				// full match
				else if (sig_idx == NULL)
					return module.begin().add(idx);
			}
		}

		// not found
		return {};
	}

	// checks if a memory region matches a signature
	inline bool match_pattern(const u8* target, const std::optional<u8>* sig, const u64 len)
	{
		for (u64 i{}; i != len; ++i)
			if (sig[i] && *sig[i] != target[i])
				return false;

		return true;
	}

	// finds all occurrences of a pattern inside a module
	inline std::vector<mem> find_all_patterns(std::string ptr, hmodule module = {})
	{
		const std::vector<u8_byte> bytes = create_bytes_from_string(ptr);
		std::vector<mem> results{};

		for (u64 i{}; i != module.size() - bytes.size(); ++i)
			if (match_pattern(module.begin().add(i).as<u8*>(), bytes.data(), bytes.size()))
				results.push_back(module.begin().add(i));

		return results;
	}

	inline void nop(u8* address, u64 size)
	{
		protect_memory_region(address, size, [=] {
			memset((void*)address, 0x90, size);
		});
	}
}