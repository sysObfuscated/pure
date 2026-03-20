#pragma once
#include "incl.h"
#include "joaat.h"

#define NET_SHOP_INVALID_TRANS_ID 0xFFFFFFFF 
typedef u64   NetShopRequestId;
typedef u32   NetShopItemId;
typedef u32   NetShopCategory;
typedef u32   NetShopTransactionId;
typedef u32   NetShopTransactionType;
typedef u32   NetShopTransactionAction;
#define NET_SHOP_INVALID_ITEM_ID  0xFFFFFFFF
//rage
namespace rage
{
	template <typename T, size_t C>
	struct __declspec(align(8)) atFixedArray
	{
		T* m_Elements[C];
		int m_Count;
	};
	template <size_t C, typename T>
	struct datPadding
	{
		unsigned __int8 m[C];
	};
	template <typename T>
	struct atArray
	{
		T* m_Elements;
		u16 m_Count;
		u16 m_Capacity;
		rage::datPadding<4, unsigned char> m_Padding;
	};
	class atString {
	public:
		const char* c_str() const {
			return m_Length ? m_Data : "";
		}

		char* m_Data;
		u16 m_Length, m_Allocated;
	};
	struct pgBaseNode {
		pgBaseNode* Next;
	};
	class pgBase {
	public:
		virtual ~pgBase() = default;
		pgBaseNode* m_FirstNode;
	};
	template <typename t, size_t count>
	class atRangeArray {
	public:
		t m_Elements[count];
	};
	inline u32 atHash(unsigned x) { return x; }
	inline u32 atHash(const void* x) { return (u32)(u64)x; }
	inline u32 atHash_const_char(const char* s) {
		u32 h = 0, g;
		if (!s)
			return 0;
		// stolen from data/hash, presumably whoever wrote
		// this knew what they were doing.
		while (*s) {
			h = (h << 4) + (*s++);
			if (((g = h & 0xf0000000)) != 0) {
				h = h ^ (g >> 24);
				h = h ^ g;
			}
		}
		return h;
	}
	inline u32 atHash_const_charU(const char* s) {
		u32 h = 0, g;
		while (*s) {
			char c = *s++;
			if (c >= 'a' && c <= 'z')
				c -= 0x20;
			h = (h << 4) + c;
			if (((g = h & 0xf0000000)) != 0) {
				h = h ^ (g >> 24);
				h = h ^ g;
			}
		}
		return h;
	}
	inline u16 atHash16(const char* s) {
		u32 hash = atHash_const_char(s);
		u32 prime = 65167;
		return u16((hash % prime) + (0xffff - prime));
	}
	inline u16 atHash16U(const char* s) {
		u32 hash = atHash_const_charU(s);
		u32 prime = 65167;
		return u16((hash % prime) + (0xffff - prime));
	}
	inline u32 atHash64(u64 key) {
		u32 h = 0, g;
		u32 count = 8;
		while (--count) {
			char s = (char)key; // take the bottom 8 bits
			key = key >> 8;

			h = (h << 4) + s;
			if (((g = h & 0xf0000000)) != 0) {
				h = h ^ (g >> 24);
				h = h ^ g;
			}
		}

		return h;
	}
	template <typename T>
	struct atMapHashFn {
		u32 operator()(const T& key) const { return atHash(key); }
	};
	template <>
	struct atMapHashFn<const char*> {
		u32 operator ()(const char* key) const { return atHash_const_char(key); }
	};
	template <> struct atMapHashFn<atString> : public atMapHashFn<const char*> {};
	struct atMapCaseInsensitiveHashFn {
		u32 operator ()(const char* key) const { return atHash_const_charU(key); }
	};
	template <>
	struct atMapHashFn<u64> {
		u32 operator ()(u64 key) const { return (atHash64(key)); }
	};
	template <typename _T>
	struct atMapEquals {
		bool operator ()(const _T& left, const _T& right) const { return left == right; }
	};
	template <>
	struct atMapEquals<const char*> {
		bool operator ()(const char* left, const char* right) const { return strcmp(left, right) == 0; }
	};
	template <> struct atMapEquals<atString> : public atMapEquals<const char*> {};
	template <>
	struct atMapEquals<u64> {
		bool operator ()(const u64 left, const u64 right) const { return (left == right); }
	};
	template <typename _Key, typename _Data>
	struct atMapEntry {
		atMapEntry(const _Key& k, atMapEntry* n) : key(k), data(), next(n) {}
		atMapEntry(const _Key& k, const _Data& d, atMapEntry* n) : key(k), data(d), next(n) {}
		_Key key;
		_Data data;
		atMapEntry* next;
		typedef atMapEntry<_Key, _Data> _ThisType;

		enum ePlaceNoneInitializer { PLACE_NONE };
		enum ePlaceKeyInitializer { PLACE_KEY };
		enum ePlaceDataInitializer { PLACE_DATA };
		enum ePlaceKeyDataInitializer { PLACE_KEY_AND_DATA };
	};
	template <typename _Key, typename _Data>
	class atMapMemory {
		typedef atMapEntry<_Key, _Data> _EntryType;
	public:
		_EntryType* Allocate(const _Key& k, _EntryType* n) {
			return new _EntryType(k, n);
		}
		_EntryType* Allocate(const _Key& k, const _Data& d, _EntryType* n) {
			return new _EntryType(k, d, n);
		}
		void DeAllocate(_EntryType* ptr) {
			delete ptr;
		}
		void AllocateHash(int slotCount, _EntryType**& hash) {
			hash = new _EntryType * [slotCount];
		}
		void DeAllocateHash(_EntryType** hash) {
			delete[] hash;
		}
	};
	struct atMapCaseInsensitiveEquals {
		bool operator ()(const char* left, const char* right) const { return 0; }
	};
	template <class _Key, class _Data, class _Hash = atMapHashFn<_Key>, class _Equals = atMapEquals<_Key>, class _MemoryPolicy = atMapMemory< _Key, _Data> >
	class atMap {
	public:
		typedef atMapEntry<_Key, _Data> Entry;
		typedef _Key KeyType;
		typedef _Data DataType;

		int GetNumSlots() const { return m_Slots; }
		int GetNumUsed() const { return m_Used; }
		Entry* GetEntry(int i) { return m_Hash[i]; }
		const Entry* GetEntry(int i) const { return m_Hash[i]; }

		class Iterator {
		private:
			friend class atMap<_Key, _Data, _Hash, _Equals, _MemoryPolicy>;
			int hashpos;
			Entry* n;
			atMap<_Key, _Data, _Hash, _Equals, _MemoryPolicy>* map;
			Iterator(atMap<_Key, _Data, _Hash, _Equals, _MemoryPolicy>& m) : hashpos(0), n(0), map(&m) { Start(); }
		public:
			void Start() {
				int localHashPos;
				int slotCount = map->GetNumSlots();
				for (localHashPos = 0; localHashPos < slotCount; localHashPos++) {
					n = map->GetEntry(localHashPos);
					if (n)
						break;
				}
				hashpos = localHashPos;
			}
			void Next() {
				if (AtEnd())
					return;
				n = n->next;
				int localHashPos = hashpos;
				const int numSlots = map->GetNumSlots();
				while (!n && (localHashPos + 1) < numSlots) {
					localHashPos++;
					n = map->GetEntry(localHashPos);
				}
				hashpos = localHashPos;
			}
			bool AtEnd() const { return !n; }
			_Key& GetKey() const { FastAssert(n); return n->key; }
			_Data& GetData() const { FastAssert(n); return n->data; }
			_Data* GetDataPtr() const { FastAssert(n); return &n->data; }
			_Data& operator*() const { return GetData(); }
			_Data* operator->() const { FastAssert(n); return &n->data; }
			Iterator& operator++() { Next(); return *this; }
			operator bool() const { return !AtEnd(); }
		};

		// PURPOSE: Class to encapsulate linear iteration through all inserted items in map, and to maintain const-only access to the items
		class ConstIterator {
		private:
			friend class atMap<_Key, _Data, _Hash, _Equals, _MemoryPolicy>;
			int hashpos;
			const Entry* cn;
			const atMap<_Key, _Data, _Hash, _Equals, _MemoryPolicy>* cmap;
			ConstIterator(const atMap<_Key, _Data, _Hash, _Equals, _MemoryPolicy>& m) : hashpos(0), cn(0), cmap(&m) { Start(); }
		public:
			void Start() {
				for (hashpos = 0; hashpos < cmap->GetNumSlots(); hashpos++) {
					cn = cmap->GetEntry(hashpos);
					if (cn) break;
				}
			}
			void Next() {
				if (AtEnd()) return;
				cn = cn->next;
				const int numSlots = cmap->GetNumSlots();
				while (!cn && (hashpos + 1) < numSlots) {
					hashpos++;
					cn = cmap->GetEntry(hashpos);
				}
			}
			bool AtEnd() const { return !cn; }
			const _Key& GetKey() const { return cn->key; }
			const _Data& GetData() const { return cn->data; }
			const _Data* GetDataPtr() const { return &cn->data; }
			const _Data& operator*() const { return GetData(); }
			const _Data* operator->() const { return &cn->data; }
			ConstIterator& operator ++ () { Next(); return *this; }
			operator bool() const { return !AtEnd(); }
		};
		Entry** m_Hash; // Storage for toplevel hash array
		u16 m_Slots; // Number of slots in toplevel hash
		u16 m_Used; // Number of those slots currently in use
		_Hash m_HashFn; // Hash functor
		_Equals m_Equals; // Equality functor
		_MemoryPolicy m_Memory;	 // Memory functor
		u8 m_AllowReCompute; //Make sure we can't do dynamic memory allocation (good for resourced maps and general dynamic memory use removal)
	};
	struct datBase {
		virtual ~datBase();
	};
	class netStatus
	{
	public:
		enum StatusCode
		{
			NET_STATUS_NONE,
			NET_STATUS_PENDING,
			NET_STATUS_FAILED,
			NET_STATUS_SUCCEEDED,
			NET_STATUS_CANCELED,
			NET_STATUS_COUNT
		};
		netStatus()
			: m_Status(NET_STATUS_NONE)
			, m_ResultCode(0)
		{
		}
		~netStatus()
		{
		}
		void Reset()
		{
		}
		StatusCode GetStatus() const
		{
			return m_StatusCode;
		}
		int GetResultCode() const
		{
			return m_ResultCode;
		}
		void SetStatus(const StatusCode code)
		{
			switch (code)
			{
			case NET_STATUS_PENDING:
			case NET_STATUS_SUCCEEDED:
				SetStatus(code, 0);
				break;
			case NET_STATUS_FAILED:
			case NET_STATUS_CANCELED:
			default:
				SetStatus(code, -1);
				break;
			}
		}
		void SetStatus(const StatusCode code, const int resultCode)
		{

		}
		void SetPending()
		{
			this->SetStatus(NET_STATUS_PENDING);
		}
		void SetSucceeded(const int resultCode = 0)
		{
			this->SetStatus(NET_STATUS_SUCCEEDED, resultCode);
		}
		void SetFailed(const int resultCode = -1)
		{
			this->SetStatus(NET_STATUS_FAILED, resultCode);
		}
		void SetCanceled()
		{
			this->SetStatus(NET_STATUS_CANCELED);
		}
		void ForceSucceeded(const int resultCode = 0)
		{

		}
		void ForceFailed(const int resultCode = -1)
		{

		}
		bool Pending() const
		{
			return NET_STATUS_PENDING == m_Status;
		}
		bool Succeeded() const
		{
			return NET_STATUS_SUCCEEDED == m_Status;
		}
		bool Failed() const
		{
			return NET_STATUS_FAILED == m_Status;
		}
		bool Canceled() const
		{
			return NET_STATUS_CANCELED == m_Status;
		}
		bool Finished() const
		{
			return Succeeded() || Failed() || Canceled();
		}
		bool None() const
		{
			return NET_STATUS_NONE == m_Status;
		}
		static const char* GetStatusCodeString(const StatusCode statusCode);
		static const char* GetStatusCodeString(const netStatus& status) { return GetStatusCodeString(status.m_StatusCode); }
		const char* c_str() const { return GetStatusCodeString(*this); }

	private:
		union
		{
			unsigned m_Status;
			StatusCode m_StatusCode;
		};
		int m_ResultCode;
	};
	struct atNamespacedHashStringBase {
		u32 m_hash;
	};
	struct  atNamespacedHashString : atNamespacedHashStringBase {};
	const struct atNonFinalHashString : atNamespacedHashString {};
	template <typename T1, typename T2>
	struct atDNode : rage::datBase
	{
		T1 Data;
		rage::atDNode<T1, T2>* Next;
		rage::atDNode<T1, T2>* Prev;
	};
	template <typename T1, typename T2>
	struct atDList
	{
		rage::atDNode<T1, T2>* Head;
		rage::atDNode<T1, T2>* Tail;
	};
}
namespace NetworkGameTransactions
{
	struct GameTransactionBase : rage::datBase
	{
		u64 m_nounce;
	};
	struct InventoryItem
	{
		int m_itemId;
		union {
			int m_inventorySlotItemId;
			int m_quantity;
		};
		int m_price;
	};
	struct GameTransactionItems : GameTransactionBase
	{
		bool m_evconly;
		int m_slot;
		rage::atArray<InventoryItem> m_items;
	};
	struct SpendEarnTransaction : GameTransactionItems
	{
		int m_bank;
		int m_wallet;
	};
	struct PlayerBalanceApplicationInfo
	{
		bool m_applicationSuccessful;
		bool m_dataReceived;
		__int64 m_bankCashDifference;
		__int64 m_walletCashDifference;
	};
	struct InventoryDataApplicationInfo
	{
		bool m_dataReceived;
		int m_numItems;
	};
	struct __declspec(align(8)) InventoryItemSet
	{
		bool m_deserialized;
		int m_slot;
		rage::atArray<InventoryItem> m_items;
		bool m_finished;
		InventoryDataApplicationInfo m_applyDataToStatsInfo;
	};
	struct __declspec(align(8)) PlayerBalance
	{
		__int64 m_evc;
		__int64 m_pvc;
		__int64 m_bank;
		__int64 m_Wallets[5];
		long double m_pxr;
		long double m_usde;
		NetworkGameTransactions::PlayerBalanceApplicationInfo m_applyDataToStatsInfo;
		bool m_deserialized;
		bool m_finished;
	};
	struct TelemetryNonce
	{
		__int64 m_nonce;
	};
}
struct CNetShopTransactionBase
{
	virtual ~CNetShopTransactionBase();
	virtual void Init() = 0;
	virtual void Cancel() = 0;
	virtual void Shutdown() = 0;
	virtual void Update() = 0;
	virtual void Clear() = 0;
	virtual void ProcessSuccess() = 0;
	virtual bool ProcessingStart() = 0;
	virtual bool GetTransactionObj(NetworkGameTransactions::SpendEarnTransaction*) = 0;
	virtual void ProcessFailure() = 0;
	virtual void SpewItems() = 0;
	virtual int GetServiceId() = 0;

	bool                      GetIsType( NetShopTransactionType type) { return (type == m_Type); }

	unsigned int m_Id;
	unsigned int m_Type;
	unsigned __int64 m_RequestId;
	unsigned int m_Category;
	rage::netStatus m_Status;
	unsigned int m_Action;
	int m_Flags;
	bool m_Checkout;
	bool m_NullTransaction;
	bool m_IsProcessing;
	unsigned int m_TimeEnd;
	NetworkGameTransactions::PlayerBalance m_PlayerBalanceResponse;
	NetworkGameTransactions::InventoryItemSet m_InventoryItemSetResponse;
	NetworkGameTransactions::TelemetryNonce m_TelemetryNonceResponse;
	unsigned int m_attempts;
	unsigned int m_FrameStart;
	unsigned int m_FrameEnd;
	unsigned int m_TimeStart;
};
struct CNetTransactionItemKey
{
	rage::atNonFinalHashString m_key;
};
struct PendingCashReductionsHelper
{
	struct __declspec(align(8)) CashAmountHelper
	{
		int m_id;
		__int64 m_bank;
		__int64 m_wallet;
		bool m_EvcOnly;
	};
	rage::atArray<CashAmountHelper> m_transactions;
	__int64 m_totalbank;
	__int64 m_totalwallet;
	__int64 m_totalbankEvcOnly;
	__int64 m_totalwalletEvcOnly;
};
typedef u32   NetShopItemId;
class CNetShopItem
{
public:
	NetShopItemId     m_Id;
	NetShopItemId     m_ExtraInventoryId;
	int               m_Price;
	int               m_StatValue;
	u32               m_Quantity;

public:
	CNetShopItem() { Clear(); }
	CNetShopItem(const NetShopItemId id, const NetShopItemId catalogId, const int price, const int statValue, const u32 quantity = 1)
		: m_Id(id)
		, m_ExtraInventoryId(catalogId)
		, m_Price(price)
		, m_StatValue(statValue)
		, m_Quantity(quantity)
	{
		;
	}

	void Clear();
	bool IsValid() const { return (m_Id != NET_SHOP_INVALID_ITEM_ID); }

	const CNetShopItem& operator=(const CNetShopItem& other)
	{
		m_Id = other.m_Id;
		m_ExtraInventoryId = other.m_ExtraInventoryId;
		m_Price = other.m_Price;
		m_StatValue = other.m_StatValue;
		m_Quantity = other.m_Quantity;

		return *this;
	}
};
struct CNetworkShoppingMgr
{
	CNetShopTransactionBase* FindBasket(NetShopTransactionId& id);
	bool AddItem(CNetShopItem& item);
	bool DeleteBasket();
	bool StartCheckout(const NetShopTransactionId id);

	#if __BANK
		unsigned int m_bankLatency;
		bool m_bankOverrideLatency;
		bool m_bankTestAsynchTransactions;
	#endif // __BANK
	//rage::atFixedArray<CNetShopTransactionBase*, 15> m_TransactionListHistory; - OUTPUT_ONLY
	/*rage::atArray<CNetTransactionItemKey> m_transactiontypes;
	rage::atArray<CNetTransactionItemKey> m_actiontypes;*/
	virtual ~CNetworkShoppingMgr() = default;
	char pad[24];
	rage::atDList<CNetShopTransactionBase*, rage::datBase> m_TransactionList;
	void* m_Allocator;
	bool m_LoadCatalogFromCache;
	bool m_transactionInProgress;
	PendingCashReductionsHelper m_cashreductions;
};
inline CNetworkShoppingMgr** NetworkShoppingMgrSingleton{};
#define NETWORK_SHOPPING_MGR (*NetworkShoppingMgrSingleton)
#pragma pack(push, 1)
class CNetShopTransactionBasket : public CNetShopTransactionBase {
public:
	CNetShopItem   m_Items[71];
	u8             m_Size;
}; //Size: 0x0670
#pragma pack(pop)
namespace rage {
	namespace sysObfuscatedTypes {
		extern u32 obfRand();
	}
#define sysMemSet memset
	// "borrowed" from rage
	struct sysMemContainerData {
		void* m_Base;
		uint32_t m_Size;
	};
	typedef struct sysIpcSemaTag* sysIpcSema;
	class datResourceChunk {
	public:
		void* SrcAddr;
		void* DestAddr;
		unsigned __int64 Size;
	};
	class datResourceMap {
	public:
		u8 VirtualCount;
		u8 PhysicalCount;
		u8 RootVirtualChunk;
		u8 DisableMerge;
		u8 HeaderType;
		void* VirtualBase;
		datResourceChunk Chunks[128];
		int LastSrc;
		int LastDest;
	};
	class sysMemDistribution {
	public:
		u32 UsedBySize[32];
		u32 FreeBySize[32];
	};
	class __declspec(align(8)) sysMemDefragmentationNode {
	public:
		void* From;
		void* To;
		unsigned int Size;
	};
	class sysMemDefragmentation {
	public:
		int Count;
		sysMemDefragmentationNode Nodes[32];
	};
	class sysMemDefragmentationFree {
	public:
		int Count;
		void* Nodes[4];
	};
	class sysMemAllocator {
	public:
		virtual ~sysMemAllocator();
		virtual void SetQuitOnFail(const bool);
		virtual void Allocate(unsigned __int64, unsigned __int64, int);
		virtual void TryAllocate(unsigned __int64, unsigned __int64, int);
		virtual void Free(const void*);
		virtual void DeferredFree(const void*);
		virtual void Resize(const void*, unsigned __int64);
		virtual void GetAllocator(int);
		virtual void GetAllocator(int) const;
		virtual void GetPointerOwner(const void*);
		virtual void GetSize(const void*);
		virtual void GetMemoryUsed(int);
		virtual void GetMemoryAvailable();
		virtual void GetLargestAvailableBlock();
		virtual void GetLowWaterMark(bool);
		virtual void GetHighWaterMark(bool);
		virtual void UpdateMemorySnapshot();
		virtual void GetMemorySnapshot(int);
		virtual void IsTallied();
		virtual void BeginLayer();
		virtual void EndLayer(const char*, const char*);
		virtual void BeginMemoryLog(const char*, bool);
		virtual void EndMemoryLog();
		virtual void IsBuildingResource();
		virtual void HasMemoryBuckets();
		virtual void SanityCheck();
		virtual void IsValidPointer(const void*);
		virtual void SupportsAllocateMap();
		virtual void AllocateMap(rage::datResourceMap*);
		virtual void FreeMap(const rage::datResourceMap*);
		virtual void GetSizeWithOverhead(const void*);
		virtual void GetHeapSize();
		virtual void GetHeapBase();
		virtual void SetHeapBase(void*);
		virtual void IsRootResourceAllocation();
		virtual void GetCanonicalBlockPtr(const void*);
		virtual void TryLockBlock(const void*, unsigned int);
		virtual void UnlockBlock(const void*, unsigned int);
		virtual void GetUserData(const void*);
		virtual void SetUserData(const void*, unsigned int);
		virtual void GetMemoryDistribution(rage::sysMemDistribution*);
		virtual void Defragment(rage::sysMemDefragmentation*, rage::sysMemDefragmentationFree*, unsigned __int64);
		virtual void GetFragmentation();
	};
	template <typename T, bool mutate = true>
	class sysObfuscated {
	public:
		void Init() {
			m_xor = sysObfuscatedTypes::obfRand();
			if (mutate) {
				m_mutate = sysObfuscatedTypes::obfRand();
			}
		}
		T Get() {
			u32 xorVal = m_xor ^ (u32)(size_t)this;
			u32 ret[sizeof(T) / sizeof(u32)];
			u32* src = const_cast<u32*>(&m_data[0]);
			u32* dest = (u32*)&ret;
			for (size_t i = 0; i < sizeof(T) / 4; ++i) {
				if (mutate) {
					// Extract valid data from two words of storage
					u32 a = *src & m_mutate;
					u32 b = src[sizeof(T) / 4] & (~m_mutate);
					// Apply entropy in the unused bits: Just flip the two u16's in the u32. We can't do a
					// huge amount more without knowledge of the mutation mask.
					u32 entropyA = ((*src & (~m_mutate)) << 16) | ((*src & (~m_mutate)) >> 16);
					u32 entropyB = ((src[sizeof(T) / 4] & m_mutate) << 16) | ((src[sizeof(T) / 4] & m_mutate) >> 16);
					*src = (*src & m_mutate) | entropyA;
					src[sizeof(T) / 4] = (src[sizeof(T) / 4] & (~m_mutate)) | entropyB;

					*dest++ = a | b;
					++src;
				}
				else {
					*dest++ = *src++ ^ xorVal;
				}
			}
			// Call Set() to reset the xor and mutate keys on every call to Get()
			if (mutate) {
				const_cast<sysObfuscated<T, mutate>*>(this)->Set(*(T*)&ret);
			}
			return *(T*)&ret;
		}
		void Set(T data) {
			Init();
			u32 xorVal = m_xor ^ (u32)(size_t)this;
			u32* src = (u32*)&data;
			u32* dest = &m_data[0];
			for (size_t i = 0; i < sizeof(T) / 4; ++i) {
				if (mutate) {
					u32 a = *src & m_mutate;
					u32 b = *src & (~m_mutate);
					++src;
					*dest = a;
					dest[sizeof(T) / 4] = b;
					++dest;
				}
				else {
					*dest++ = *src++ ^ xorVal;
				}
			}
		}
		void operator=(T data) {
			Set(data);
		}
		operator T() {
			return Get();
		}
	private:
		u32 m_data[(mutate ? sizeof(T) * 2 : sizeof(T)) / sizeof(u32)];
		// XOR and mutate keys for this type
		u32 m_xor;
		u32 m_mutate;
	};
}
// scr
namespace rage {
	enum scrProgramId { srcpidNONE };
	union scrValue {
		enum ValueType { UNKNOWN, BOOL, INT, FLOAT, VECTOR, TEXT_LABEL, STRING, OBJECT };
		enum VarArgType { VA_INT, VA_FLOAT, VA_STRINGPTR, VA_VECTOR };
		int Int;
		unsigned Uns;
		float Float;
		const char* String;
		scrValue* Reference;
		size_t Any;
		bool operator==(const scrValue& that) {
			return Int == that.Int;
		}
	};
	typedef scrValue* scrReference;
	typedef size_t scrAny;
	enum scrThreadId : int {
		THREAD_INVALID = 0,
	};
	struct scrVector {
		scrVector() : x(0), y(0), z(0) {}
		scrVector(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
		void set(float sx, float sy, float sz) {
			x = sx;
			y = sy;
			z = sz;
		}
		scrVector& operator=(const scrVector& v) {
			x = v.x; y = v.y; z = v.z;
			return *this;
		}
		scrVector operator*(const float amount) {
			return { x * amount, y * amount, z * amount };
		}
		scrVector operator+(const scrVector vec) const {
			return { x + vec.x, y + vec.y, z + vec.z };
		}
		float x, xPad, y, yPad, z, zPad;
		scrVector negate() const {
			return scrVector(-x, -y, -z);
		}
	};
#define MAX_CALLSTACK 16
	class scrThread {
	public:
		enum { c_DefaultStackSize = 512 }; // Static const causes missing symbol under gcc 4.1.1 debug builds.
		// Presume it's something to do with its use as a default parameter.
		enum { c_NativeInsnLength = 4 }; // Length of OP_NATIVE insn in case caller needs PC of *next* insn.
		enum State { RUNNING, BLOCKED, ABORTED, HALTED };
		enum Priority { THREAD_PRIO_HIGHEST, THREAD_PRIO_NORMAL, THREAD_PRIO_LOWEST, THREAD_PRIO_MANUAL_UPDATE = 100 };
		struct Serialized {
			scrThreadId m_ThreadId; // +0 Unique thread ID (always increasing)
			scrProgramId m_Prog; // +4 Stored by hashcode rather than pointer so load/save can work
			State m_State; // +8 Current thread state
			int m_PC; // +12 Current program counter (index into program's opcodes array)
			int m_FP; // +16 Current frame pointer (anchor for local variables)
			int m_SP; // +20 Stack pointer
			float m_TimerA, m_TimerB; // Local per-thread timer values
			float m_Wait; // Accumulated wait timer
			int m_MinPC, m_MaxPC; // Step region for debugger
			atRangeArray<scrValue, 4> m_TLS; // Thread-local storage for blocking functions
			int m_StackSize;
			int m_CatchPC, m_CatchFP, m_CatchSP;
			Priority m_Priority; // 0=highest (runs first), 2=lowest (runs last)
			s8 m_CallDepth;
			char m_CallStack[16];
		};
		struct Global {
			int BasicType; // enumerant in scrValue
			int Offset; // Base address of this variable
			u32 Hash; // Hashed name of this variable (as per scrComputeHash)
		};
		struct Info {
			Info(scrValue* resultPtr, int parameterCount, scrValue* params) : ResultPtr(resultPtr), ParamCount(parameterCount), Params(params), BufferCount(0) {}
			// Return result, if applicable
			scrValue* ResultPtr;
			// Parameter count
			int ParamCount;
			// Pointer to parameter values
			scrValue* Params;
			// Temp storage for managing copyin/copyout of Vector3 parameters
			int BufferCount;
			enum { MAX_VECTOR3 = 4 };
			scrValue* Orig[MAX_VECTOR3];
			scrVector Buffer[MAX_VECTOR3];
			scrVector& GetVector3(int& N) {
				scrValue* v = Params[N++].Reference;
				Orig[BufferCount] = v;
				Buffer[BufferCount].set(v[0].Float, v[1].Float, v[2].Float);
			}
			void CopyReferencedParametersOut() {
				int bc = BufferCount;
				while (bc--) {
					Orig[bc][0].Float = Buffer[bc].x;
					Orig[bc][1].Float = Buffer[bc].y;
					Orig[bc][2].Float = Buffer[bc].z;
				}
			}
			template <typename T>
			constexpr T& get(size_t index) {
				static_assert(sizeof(T) <= sizeof(uint64_t));
				return *reinterpret_cast<T*>(reinterpret_cast<uint64_t*>(Params) + index);
			}
			template <typename T>
			void set(size_t index, T&& value)
			{
				static_assert(sizeof(T) <= sizeof(uint64_t));
				*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<T>>*>(reinterpret_cast<uint64_t*>(Params) + index) = std::forward<T>(value);
			}
			template <typename T>
			void set_ret_value(T&& value)
			{
				*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<T>>*>(ResultPtr) = std::forward<T>(value);
			}
		};
		virtual ~scrThread() = default;
		virtual void Reset(scrProgramId program, const void* argStruct, int argStructSize) {}
		virtual State Run(int insnCount) { return m_Serialized.m_State; }
		virtual State Update(int insnCount) { return m_Serialized.m_State; }
		virtual void Kill() {}
		struct ThreadStack {
			scrThread* m_Owner;
			int m_StackSize;
			scrValue* m_Stack;
		};
		Serialized m_Serialized;
		scrValue* m_Stack;
		int	m_iInstructionCount;
		int m_argStructSize;
		int m_argStructOffset;
		const char* m_AbortReason;
		u32 m_ScriptNameHash;
		char m_ScriptName[64];
	};
	typedef void(*scrCmd)(scrThread::Info*);
	const int MAX_LEGIT_OPS = (11 * 1024 * 1024); //	Used as the size of the atFixedArray called Program in rage\script\tools\scriptcompiler\node.h

	// These configure the size of each opcode page
	static constexpr u32 scrPageShift = 14;
	static constexpr u32 scrPageSize = (1 << scrPageShift);
	static constexpr u32 scrPageMask = (scrPageSize - 1);

	// These configure the size of each string page.
	static constexpr u32 scrStringShift = 14;
	static constexpr u32 scrStringSize = (1 << scrStringShift);
	static constexpr u32 scrStringMask = (scrStringSize - 1);

	// These configure the size of each globals page, in multiples of sizeof(scrValue).  This is used to prevent globals from requiring an absurdly large memory block just long enough to (re)load.
	static constexpr u32 scrGlobalsPageShift = 14;
	static constexpr u32 scrGlobalsPageSize = (1 << scrGlobalsPageShift);
	static constexpr u32 scrGlobalsPageMask = (scrGlobalsPageSize - 1);

	class scrProgram : public pgBase {
		friend class scrThread;
	public:
		static const int RORC_VERSION = 12;

		enum {
			MAX_GLOBAL_BLOCKS_SHIFT = (24 - 6),
			MAX_GLOBAL_BLOCKS = (64),
			GLOBAL_SIZE_MASK = ((1 << (MAX_GLOBAL_BLOCKS_SHIFT)) - 1)
		};

		u8** Table; // +8 (zero if compiled)
		u32 GlobalsHash; // +12
		u32 OpcodeSize;	// +16
		u32 ArgStructSize; // +20
		u32 StaticSize;	// +24
		u32 GlobalSizeAndBlock;// +28 - upper bits are the globals block index
		u32 NativeSize; // +32
		scrValue* Statics; // +36
		scrValue** GlobalsTable; // +40 (zero if compiled, globals cannot be declared in a compiled script)
		scrCmd* Natives; // +44 (if compiled, really a script function pointer)
		u32 ProcCount; // +48 (zero if compiled)
		const char** ProcNames; // +52 (zero if compiled)
		u32 HashCode; // +56
		u32 RefCount; // +60
		const char* ScriptName; // +64
		const char** StringHeaps; // +68 (zero if compiled)
		u32 StringHeapSize; // +72 (zero if compiled)
		atMap<s32, bool>* m_programBreakpoints; // +76 (zero if compiled)

		u8* GetCode(u32 index) {
			if (index < OpcodeSize) {
				return &Table[index >> scrPageShift][index & scrPageMask];
			}
			return nullptr;
		}
		const scrValue* GetStatics() const { return Statics; }
		int GetStaticSize() const { return StaticSize; }
		int GetStringHeapSize() const { return StringHeapSize; }
		u32 GetHashCode() const { return HashCode; }
		u32 GetNativeSize() const { return NativeSize; }
		const scrCmd* GetNatives() const { return Natives; }
		u32 GetStringHeapCount() const { return (StringHeapSize + scrStringMask) >> scrStringShift; }
		u32 GetStringHeapChunkSize(u32 i) { return i == GetStringHeapCount() - 1 ? StringHeapSize - (i << scrStringShift) : scrStringSize; }
		u32 GetGlobalsPageCount() const { return ((GlobalSizeAndBlock & GLOBAL_SIZE_MASK) + scrGlobalsPageMask) >> scrGlobalsPageShift; }
		u32 GetGlobalsPageChunkSize(u32 i) { return i == GetGlobalsPageCount() - 1 ? (GlobalSizeAndBlock & GLOBAL_SIZE_MASK) - (i << scrGlobalsPageShift) : scrGlobalsPageSize; }
	};
	template <typename T>
	class scrCommandHash {
	private:
		static const int ToplevelSize = 256;	// Must be power of two
		static const int PerBucket = 7;
		struct Bucket {
			sysObfuscated<Bucket*, false> obf_Next;
			T Data[PerBucket];
			sysObfuscated<u32, false> obf_Count;
			sysObfuscated<u64, false> obf_Hashes[PerBucket];
			u64 plainText_Hashes[PerBucket];
		};
	public:
		void RegistrationComplete(bool val) {
			m_bRegistrationComplete = val;
		}
		void Init() {
			m_Occupancy = 0;
			m_bRegistrationComplete = false;
			for (int i{}; i < ToplevelSize; i++)
				m_Buckets[i] = NULL;
		}
		void Kill() {
			for (int i = 0; i < ToplevelSize; i++) {
				Bucket* b = m_Buckets[i];
				while (b) {
					char* old = (char*)b;
					b = b->obf_Next.Get();
					delete[] old;
				}
				m_Buckets[i] = NULL;
			}
			m_Occupancy = 0;
		}
		T Lookup(u64 hashcode) {
			Bucket* b = m_Buckets[hashcode & (ToplevelSize - 1)];
			while (b) {
				for (u32 i{}; i != b->obf_Count.Get(); i++)
					if (b->obf_Hashes[i].Get() == hashcode)
						return b->Data[i];
				b = b->obf_Next.Get();
			}
			return T{};
		}
		Bucket* m_Buckets[ToplevelSize];
		int m_Occupancy;
		bool m_bRegistrationComplete;
	};
	class tlsContext {
	public:
		char gap0[180];
		std::uint8_t m_unk_byte; // 0xB4
		char gapB5[3];
		sysMemAllocator* m_allocator; // 0xB8
		sysMemAllocator* m_allocator2; // 0xC0 - Same as 0xB8
		sysMemAllocator* m_allocator3; // 0xC8 - Same as 0xB8
		uint32_t m_console_smth; // 0xD0
		char gapD4[188];
		uint64_t m_unk; // 0x190
		char gap198[1728];
		scrThread* m_script_thread; // 0x858
		bool m_is_script_thread_active; // 0x860

		static tlsContext* get() {
			constexpr std::uint32_t TlsIndex = 0x0;
			return *reinterpret_cast<tlsContext**>(__readgsqword(0x58) + TlsIndex);
		}
	};
}