#pragma once
#include "memory/util.h"

namespace pure::memory
{
	struct hook
	{
		hook(void* target, void* detour) : m_target(target), m_detour(detour)
		{
			if (target)
			{
				memcpy(m_original, m_target, g_shellcode_size);
				enable();
			}
		}
		void enable()
		{
			protect_memory_region(m_target, g_shellcode_size, [&]
			{
				memcpy(m_target, g_shellcode, g_shellcode_size);
				memcpy((char*)m_target + (sizeof(void*) == 8 ? 2 : 1), &m_detour, sizeof(void*));
			});
		}
		void disable()
		{
			if (m_target)
			{
				protect_memory_region(m_target, g_shellcode_size, [&]
				{
					memcpy(m_target, m_original, g_shellcode_size);
				});
			}
		}
		template <typename T = void, typename... Args>
		T call(Args... args)
		{
			disable();
			if constexpr (!std::is_same_v<T, void>) {
				T retval{ std::invoke((T(*)(Args...))(m_target), std::forward<Args>(args)...) };
				enable();
				return retval;
			}
			std::invoke((T(*)(Args...))(m_target), std::forward<Args>(args)...);
			enable();
		}

		void* m_target{};
		void* m_detour{};
		char m_original[g_shellcode_size]{};
	};

	class hooks final
	{
	public:
		hooks() = default;
		~hooks() = default;
	public:
		void create();
		void destroy();
	public:
		hook* add(void* ptr, void* detour)
		{
			auto hk{ new hook(ptr, detour) };
			m_hooks.push_back(hk);
			return hk;
		}
		bool empty() 
		{
			return m_hooks.empty();
		}
	public:
		std::vector<hook*> m_hooks{};
	};

	inline hooks g_hooks{};
}