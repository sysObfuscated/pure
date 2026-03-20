#pragma once
#include "memory/pointers.h"

namespace pure
{
	struct command_hook
	{
		void enable()
		{
			pointers::g_command_hash_insert(pointers::g_command_hash, m_hash, m_detour);
		}
		void disable()
		{
			pointers::g_command_hash_insert(pointers::g_command_hash, m_hash, m_og);
		}
		void call(rage::scrThread::Info* cmd)
		{
			m_og(cmd);
		}

		u64 m_hash{};
		rage::scrCmd m_og{};
		rage::scrCmd m_detour{};
	};

	class command_hooks final
	{
	public:
		command_hooks() = default;
		~command_hooks() = default;
	public:
		command_hook* add(u64 hash, rage::scrCmd detour)
		{
			auto hk{ new command_hook(hash, pointers::g_command_hash->Lookup(hash), detour) };
			hk->enable();
			m_hooks.push_back(hk);
			return hk;
		}
		void remove()
		{
			for (auto& h : m_hooks)
				h->disable();
		}
	public:
		std::vector<command_hook*> m_hooks{};
	};

	inline command_hooks g_command_hooks{};

	inline void empty_hook(rage::scrThread::Info* cmd) {}
}