#ifndef MONKEY_HASH_MAP
#define MONKEY_HASH_MAP

#define DEFAULT_LOAD_FACTOR 0.5
#define DEFAULT_MAX_HASHES 50

#include <random>
#include <functional>
#include <list>
#include "MonkeyHashMapEntry.hpp"

template <class K, class V>
class MonkeyHashMap
{
private:
	std::random_device random;

	MonkeyHashMapEntry<K,V> **entries;
	int maxCapacity;
	int size;
	int arrayLength;
	int bitMaskForSmartMod;
	int maxHashes;
	int hashesInUse;
	int *entryCountByNumberOfHashesUsed;

	std::function<K(V)> valueToKeyFunction;

	MonkeyHashMapEntry<K,V>* getEntry(K key, bool upsertIntended);
	void removeEntry(MonkeyHashMapEntry<K,V>*);
	bool validateMapping(K key, V value);
	static int getNextPowerOfTwo(int value);
	void addContributionToNumberOfHashesUsed(MonkeyHashMapEntry<K,V>*);
	void removeContributionToNumberOfHashesUsed(MonkeyHashMapEntry<K,V>*);

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
	void iterateThroughEntries();

	void print();
	~MonkeyHashMap();
};

#endif
