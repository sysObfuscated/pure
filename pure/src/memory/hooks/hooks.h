#pragma once
#include "memory/components/hook.h"
using namespace pure::memory;

namespace pure
{
	extern __int64 can_access_network_features(__int64 a1, u32* a2);
	extern bool network_bail(int bail_params, bool send_script_event);

	inline hook* g_can_access_network_features{};
	inline hook* g_network_bail{};
}