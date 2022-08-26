#ifndef MONKEY_HASH_MAP
#define MONKEY_HASH_MAP

#define DEFAULT_LOAD_FACTOR 0.5
#define DEFAULT_MAX_HASHES 50

#include <random>
#include <functional>

template <class K, class V>
class MonkeyHashMap
{
	private:
	std::random_device random;

	class MonkeyHashMapEntry;
	MonkeyHashMapEntry **entries;
	int maxCapacity;
	int size;
	int arrayLength;
	int bitMaskForSmartMod;
	int maxHashes;
	int hashesInUse;
	int *entryCountByNumberOfHashesUsed;

	std::function<K(V)> valueToKeyFunction;

	MonkeyHashMapEntry* getEntry(K key, bool upsertIntended);
	void removeEntry(MonkeyHashMapEntry*);
	bool validateMapping(K key, V value);
	static int getNextPowerOfTwo(int value);
	void addContributionToNumberOfHashesUsed(MonkeyHashMapEntry*);
	void removeContributionToNumberOfHashesUsed(MonkeyHashMapEntry*);

	class MonkeyHashMapEntry
	{
		public:
		K key;
		V value;
		int numberOfHashesUsed;
		int positionInArray;

		MonkeyHashMapEntry(K key, V value, int positionInArray);
	};

	public:
	MonkeyHashMap(int maxCapacity);
	MonkeyHashMap(int maxCapacity, float loadFactor);
	MonkeyHashMap(int maxCapacity, K valueToKeyFunction(V));
	MonkeyHashMap(int maxCapacity, float loadFactor, K valueToKeyFunction(V));
	MonkeyHashMap(int maxCapacity, float loadFactor, K valueToKeyFunction(V), int maxHashes);
	int getSize();
	bool isEmpty();
	bool containsKey(K);
	bool containsValue(V);
	V get(K);
	V put(K, V);
	V remove(K);
	void clear();
	int getHashesInUse();

	void print();
	~MonkeyHashMap();

	class Iterator
	{
		private:
		int i;
		int arrayLength;
		MonkeyHashMapEntry **entries;
		MonkeyHashMap &parent;

		public:
		Iterator(MonkeyHashMap &x);
		bool hasNext();
		K next();
	};
};

#endif
