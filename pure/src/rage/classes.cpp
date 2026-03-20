#include "classes.h"
#include "memory/pointers.h"

// r* dont sue me pls
CNetShopTransactionBase* CNetworkShoppingMgr::FindBasket(NetShopTransactionId& id)
{
	rage::atDNode<CNetShopTransactionBase*, rage::datBase>* pNode = m_TransactionList.Head;
	rage::atDNode<CNetShopTransactionBase*, rage::datBase>* pNextNode = 0;

	id = NET_SHOP_INVALID_TRANS_ID;

	while (pNode)
	{
		pNextNode = pNode->Next;

		CNetShopTransactionBase* pEvent = pNode->Data;

		if (pEvent->GetIsType("NET_SHOP_TTYPE_BASKET"_joaat))
		{
			id = pEvent->m_Id;
			return pEvent;
		}

		pNode = pNextNode;
	}


	return 0;
}
bool CNetworkShoppingMgr::AddItem(CNetShopItem& item)
{
	bool result = false;

	NetShopTransactionId id = NET_SHOP_INVALID_TRANS_ID;
	CNetShopTransactionBase* pEvent = FindBasket(id);

	if (pEvent)
	{
		// @TODO: ADD LOG
		//gnetDebug1("[%u] [%" I64FMT "d] AddItem - Transaction type=\"%s\" status=\"%s\", add Id=\"%d\", ExtraInventoryId=\"%d\"", pEvent->GetId(), pEvent->GetRequestId(), pEvent->GetTypeName(), pEvent->GetStatusString(), item.m_Id, item.m_ExtraInventoryId);

		if (pEvent->m_Status.None())
		{
			CNetShopTransactionBasket* transaction = static_cast<CNetShopTransactionBasket*> (pEvent);
			if (transaction)
			{
				result = pure::pointers::g_basket_add(transaction, item);

				//We added the item successfully - update pending cash
				if (result)
				{
					//We only register at the end of buying the property due to property trading.
					//This will be done when the checkout is called.
					if ("NET_SHOP_ACTION_BUY_PROPERTY"_joaat != transaction->m_Action && "NET_SHOP_ACTION_BUY_WAREHOUSE"_joaat != transaction->m_Action)
					{
						pure::pointers::g_register_pending_cash(transaction);
					}
				}
			}
		}
	}

	return result;
}
bool CNetworkShoppingMgr::DeleteBasket() 
{
	bool result{};
	//Creating free spaces: delete basket
	rage::atDNode<CNetShopTransactionBase*, rage::datBase>* node{ m_TransactionList.Head };
	rage::atDNode<CNetShopTransactionBase*, rage::datBase>* next_node{};

	while (node) 
	{
		next_node = node->Next;
		CNetShopTransactionBase* event{ node->Data };

		if (event->m_Type == "NET_SHOP_TTYPE_BASKET"_joaat) 
		{
			result = (!event->m_Status.Pending());
			if (result) 
			{
				delete event;
				m_TransactionList.Head = 0;
				m_TransactionList.Tail = 0;
				delete node;
				break;
			}
		}

		node = next_node;
	}

	return result;
}
bool CNetworkShoppingMgr::StartCheckout(const NetShopTransactionId id) 
{
	bool result{};
	rage::atDNode<CNetShopTransactionBase*, rage::datBase>* pNode{ m_TransactionList.Head };
	rage::atDNode<CNetShopTransactionBase*, rage::datBase>* pNextNode{};
	while (pNode) 
	{
		pNextNode = pNode->Next;
		CNetShopTransactionBase* pEvent{ pNode->Data };

		if (id == pEvent->m_Id) 
		{
			if (!pEvent->m_Status.Pending() && !pEvent->m_Status.Succeeded() && !pEvent->m_Status.Canceled() && !pEvent->m_Checkout) 
			{
				pEvent->m_Checkout = true;
				pEvent->m_Status.Reset();
				pEvent->m_TimeEnd = 0;
				//Check if this is a is a property trade and is not a credit.
				bool is_trade_credit{};

				if (pEvent->m_Type == "NET_SHOP_TTYPE_BASKET"_joaat) 
				{
					const CNetShopTransactionBasket* transaction = static_cast<const CNetShopTransactionBasket*> (pEvent);

					if (transaction) 
					{
						int numProperties{};
						bool hasOverrideItem{};
						//transaction->GetTotalPrice(is_trade_credit, numProperties, hasOverrideItem);
						// not doing allat lol
					}
				}

				if (!is_trade_credit)
					pure::pointers::g_register_pending_cash(pEvent);

				result = true;
			}

			pNextNode = 0;
		}
		pNode = pNextNode;
	}
	return result;
}