#include "components/command_hook.h"
#include "util.h"

#define SCAN(p, a, ...) p = memory::scan(a)__VA_ARGS__.as<decltype(p)>();
#define SCR_REGISTER_UNUSED(hash) g_command_hooks.add(hash, &empty_hook);

namespace pure::pointers
{
	void arxan_bs()
	{
		s32 count{};

		auto xrefs = memory::find_all_patterns("48 8D 45 ? 48 89 45 ? 48 8D 05 ? ? ? ? 48 89 45");

		for (mem& r : xrefs)
		{
			memory::nop(r.add(8).as<u8*>(), 7);
			++count;
		}
	}

	void scan_all()
	{
		SCAN(g_basket_add, "83 79 1C 00 75 50 8A 81 ? ? ? ? 3C 47 73 46 0F B6 C0 4C 8D 04 80");
		SCAN(g_register_pending_cash, "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 48 8B EC 48 83 EC 60 8B 41 24 48 8B D9 40 B7 01");
		SCAN(g_create_basket, "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 54 41 56 41 57 48 83 EC 30 48 8B F1");
		SCAN(g_network_shopping_mgr, "48 8B 0D ? ? ? ? 89 74 24 30", .qword());
		SCAN(g_sys_obf_rand_next, "8B 0D ? ? ? ? BA ? ? ? ? 69 C9", .dword());
		SCAN(g_command_hash, "48 8D 0D ? ? ? ? 48 8B 14 FA", .qword());
		SCAN(g_command_hash_insert, "E8 ? ? ? ? 4C 8D 05 ? ? ? ? 48 BA ? ? ? ? ? ? ? ? 48 8B CB 48 83 C4 20", .call());
		SCAN(g_can_access_network_features, "E9 ? ? ? ? 4E F6 03 63", .call());
		SCAN(g_network_bail, "E8 ? ? ? ? 32 C0 EB 11", .call());

		arxan_bs();
	}

	void command_hooks()
	{
		// essentially yeets the function from the game via regstering a new empty cmd using the same hash

		SCR_REGISTER_UNUSED(0x7F7E8401F81CB65B); // get game restart reason
	}
}

namespace rage
{
	namespace sysObfuscatedTypes
	{
		u32 obfRand()
		{
			u32& next{ *pure::pointers::g_sys_obf_rand_next };
			next = next * 214013 + 2531011;
			return next;
		}
	}
}