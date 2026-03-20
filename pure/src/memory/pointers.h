#pragma once
#include "rage/classes.h"

namespace pure::pointers
{
	namespace types
	{
		using basket_add = bool(*)(CNetShopTransactionBasket* _this, const CNetShopItem& item);
		using register_pending_cash = bool(*)(CNetShopTransactionBase* _this);
		using create_basket = bool(*)(CNetworkShoppingMgr* pTransactionMgr, NetShopTransactionId& pId, u32 Category, u32 Action, u32 Flags);
		using command_hash_insert = void(*)(rage::scrCommandHash<rage::scrCmd>* _this, __int64 hashcode, rage::scrCmd cmd);
	}

	extern void scan_all();
	extern void command_hooks();

	inline CNetworkShoppingMgr** g_network_shopping_mgr{};
	inline types::basket_add g_basket_add{};
	inline types::register_pending_cash g_register_pending_cash{};
	inline types::create_basket g_create_basket{};
	inline types::command_hash_insert g_command_hash_insert{};
	inline u32* g_sys_obf_rand_next{};
	inline rage::scrCommandHash<rage::scrCmd>* g_command_hash{};
	inline void* g_can_access_network_features{};
	inline void* g_network_bail{};
}