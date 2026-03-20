#pragma once
#include "memory/pointers.h"
#include "util/logger.h"

namespace pure::network
{
	// checks if there is currently an active basket/transaction
	bool basket_is_active() 
	{
		NetShopTransactionId id = NET_SHOP_INVALID_TRANS_ID;
		CNetShopTransactionBase* basket = (*pointers::g_network_shopping_mgr)->FindBasket(id);

		// if basket is not null, a transaction is active
		return (NULL != basket);
	}

	// starts a network transaction
	bool start_transaction(std::string category, std::string action, std::string id, int amount, int flag, u32 quanity) 
	{
		NetShopTransactionId transaction_id = NET_SHOP_INVALID_TRANS_ID;
		CNetworkShoppingMgr* mgr = *pointers::g_network_shopping_mgr;

		// dont start if another transaction is already in progress
		if (!mgr->m_transactionInProgress) 
		{
			// clear any existing basket
			if (mgr->DeleteBasket())
				LOG("Network", "Deleted current basket");

			// create a new basket w/ category etc
			if (pointers::g_create_basket(mgr, transaction_id, rage::joaat(category), rage::joaat(action), flag))
			{
				// build shop item & add it to the baskte
				CNetShopItem item = { rage::joaat(id), 1, amount, 0, quanity };
				mgr->AddItem(item);

				// attempt checkout
				if (mgr->StartCheckout(transaction_id))
					return true;
			}
		}

		LOG("Network", "Failed");

		// failed to start transaction
		return false;
	}

	// simple test to get 180k
	bool transaction_test()
	{
		return start_transaction("CATEGORY_SERVICE_WITH_THRESHOLD", "NET_SHOP_ACTION_EARN", "SERVICE_EARN_JUGGALO_STORY_MISSION", 180000, 4, 1);
	}
}