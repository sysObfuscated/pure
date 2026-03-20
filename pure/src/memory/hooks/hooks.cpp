#include "hooks.h"
#include "memory/pointers.h"
#include "memory/components/command_hook.h"

namespace pure
{
	__int64 can_access_network_features(__int64 a1, u32* a2)
	{
		if (a2)
			*a2 = 0;

		return true;
	}

	bool network_bail(int bail_params, bool send_script_event)
	{
		return false;
	}

	void hooks::create()
	{
		add(pointers::g_can_access_network_features, can_access_network_features);
		add(pointers::g_network_bail, network_bail);
	}

	void hooks::destroy()
	{
		if (!empty())
		{
			for (auto& h : m_hooks)
				h->disable();

			m_hooks.clear();
		}

		g_command_hooks.remove();
	}
}