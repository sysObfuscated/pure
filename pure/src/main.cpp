#include "incl.h"
#include "util/logger.h"
#include "memory/pointers.h"
#include "memory/hooks/hooks.h"
#include "network/shopping.h"

namespace pure
{
	void loop()
	{
		while (g_running)
		{
			if (GetAsyncKeyState(VK_END))
				g_running = false;

			if (GetAsyncKeyState(VK_F12) & 0x1)
				network::transaction_test();

			std::this_thread::sleep_for(100ms);
		}
	}

	DWORD WINAPI entry(void* hmod)
	{
		g_entry = reinterpret_cast<HMODULE>(hmod);
		g_logger.create();
		pointers::scan_all();
		pointers::command_hooks();
		g_hooks.create();

		loop();

		g_logger.destroy();
		g_hooks.destroy();

		FreeLibraryAndExitThread(g_entry, 0);
		return 0;
	}
}

BOOL APIENTRY DllMain(HMODULE hmod, DWORD  reason, LPVOID)
{
	switch (reason) {
	case DLL_PROCESS_ATTACH: {
		CreateThread(nullptr, NULL, &pure::entry, hmod, NULL, nullptr);
	} break;
	case DLL_PROCESS_DETACH: {
		g_running = false;
	} break;
	}
	return TRUE;
}

