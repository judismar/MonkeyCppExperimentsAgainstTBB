#include "MonkeyHashMap.hpp"
#include <cmath>
#include <boost/functional/hash/hash.hpp>
#include <stdexcept>
#include <iostream>

using namespace std;

template <class K, class V>
MonkeyHashMap<K,V>::MonkeyHashMap(int maxCapacity) : MonkeyHashMap(maxCapacity, DEFAULT_LOAD_FACTOR){}

template <class K, class V>
MonkeyHashMap<K,V>::MonkeyHashMap(int maxCapacity, float loadFactor) : MonkeyHashMap(maxCapacity, loadFactor, NULL){}

template <class K, class V>
MonkeyHashMap<K,V>::MonkeyHashMap(int maxCapacity, K valueToKeyFunction(V)) : MonkeyHashMap(maxCapacity, DEFAULT_LOAD_FACTOR, valueToKeyFunction){}

template <class K, class V>
MonkeyHashMap<K,V>::MonkeyHashMap(int maxCapacity, float loadFactor, K valueToKeyFunction(V)) : MonkeyHashMap(maxCapacity, loadFactor, valueToKeyFunction, DEFAULT_MAX_HASHES){}

template <class K, class V>
MonkeyHashMap<K,V>::MonkeyHashMap(int maxCapacity, float loadFactor, K valueToKeyFunction(V), int maxHashes)
{
	this->maxCapacity = maxCapacity;
	this->arrayLength = getNextPowerOfTwo(1 + (int)ceil(maxCapacity/loadFactor));
	this->bitMaskForSmartMod = this->arrayLength - 1;
	this->entries = new MonkeyHashMapEntry<K,V>* [this->arrayLength];
	for(int i = 0; i < this->arrayLength; i++) this->entries[i] = NULL;
	this->maxHashes = maxHashes;
	this->size = 0;
	this->hashesInUse = 0;
	this->entryCountByNumberOfHashesUsed = new int[maxHashes + 1];
	this->valueToKeyFunction = valueToKeyFunction;
}

template <class K, class V>
int MonkeyHashMap<K,V>::getSize()
{
	return size;
}

template <class K, class V>
bool MonkeyHashMap<K,V>::isEmpty()
{
	return size == 0;
}

template <class K, class V>
bool MonkeyHashMap<K,V>::containsKey(K key)
{
	return getEntry(key, false) != NULL;
}

template <class K, class V>
bool MonkeyHashMap<K,V>::containsValue(V value)
{
	MonkeyHashMapEntry<K,V> *entry;
	for(int i = 0; i < arrayLength; i++)
	{
		entry = entries[i];
		if(entry != NULL && entry->getValue() == value) return true;
	}
	return false;
}

template <class K, class V>
V MonkeyHashMap<K,V>::get(K key)
{
	MonkeyHashMapEntry<K,V> *entry = getEntry(key, false);
	if(entry == NULL) return -1;
	V value = entry->getValue();
	if (!validateMapping(key, value)) return -1;
	return value;
}

template <class K, class V>
bool MonkeyHashMap<K,V>::validateMapping(K key, V value)
{
	if(value == -1) return false;
	return valueToKeyFunction == NULL || key == valueToKeyFunction(value);
}

template <class K, class V>
V MonkeyHashMap<K,V>::put(K key, V value)
{
	MonkeyHashMapEntry<K,V> *entry = getEntry(key, true);
	if(entry == NULL) throw std::invalid_argument("Put operation failed.");
	K newEntryKey = entry->getKey();
	if(newEntryKey == "")
	{
		if(size == maxCapacity) throw std::invalid_argument("The maximum capacity has been reached.");
		size++;
		entry->setKey(key);
	}
	V oldValue = entry->getValue();
	entry->setValue(value);
	return oldValue;
}

template <class K, class V>
V MonkeyHashMap<K,V>::remove(K key)
{
	MonkeyHashMapEntry<K,V> *entry = getEntry(key, false);
	if(entry != NULL)
	{
		V value = entry->getValue();
		removeEntry(entry);
		return value;
	}
	return -1;
}

template <class K, class V>
void MonkeyHashMap<K,V>::clear()
{
	for(int i = 0; i < arrayLength; i++)
	{
		delete entries[i];
		entries[i] = NULL;
	}
	size = 0;
	for(int i = 1; i <= hashesInUse; i++) entryCountByNumberOfHashesUsed[i] = 0;
	hashesInUse = 0;
}

template <class K, class V>
int MonkeyHashMap<K,V>::getHashesInUse()
{
	return hashesInUse;
}

template <class K, class V>
void MonkeyHashMap<K,V>::iterateThroughEntries()
{
	for(int i = 0; i < arrayLength; i++)
		if(entries[i] != NULL){}
}

template <class K, class V>
MonkeyHashMapEntry<K,V>* MonkeyHashMap<K,V>::getEntry(K key, bool upsertIntended)
{
	int firstAvailablePosition = -1;
	int hashNumberUsedForFirstAvailablePosition = 0;
	int maxHashesForLookup = upsertIntended ? maxHashes : hashesInUse;

	for(int hashNumber = 1; hashNumber <= maxHashesForLookup; hashNumber++)
	{
		std::size_t hash = 0;
		boost::hash_combine(hash, hashNumber);
		boost::hash_combine(hash, key);
		hash = hash & bitMaskForSmartMod;
		MonkeyHashMapEntry<K,V> *entry = entries[hash];
		if(entry != NULL && entry->getKey() == key) return entry;
		if(upsertIntended)
		{
			if(firstAvailablePosition == -1 && (entry == NULL || entry->getKey() == ""))
			{
				firstAvailablePosition = hash;
				hashNumberUsedForFirstAvailablePosition = hashNumber;
			}
			if(firstAvailablePosition != -1 && hashNumber > hashesInUse) break;
		}
	}
	if(!upsertIntended) return NULL;
	if(firstAvailablePosition == -1) return NULL;
	MonkeyHashMapEntry<K,V> *availableEntry = entries[firstAvailablePosition];
	if(availableEntry == NULL)
	{
		availableEntry = new MonkeyHashMapEntry<K,V>("", -1, firstAvailablePosition);
		entries[firstAvailablePosition] = availableEntry;
	}
	availableEntry->setNumberOfHashesUsed(hashNumberUsedForFirstAvailablePosition);
	addContributionToNumberOfHashesUsed(availableEntry);
	return availableEntry;
}

template <class K, class V>
void MonkeyHashMap<K,V>::removeEntry(MonkeyHashMapEntry<K,V> *entry)
{
	removeContributionToNumberOfHashesUsed(entry);
	if(valueToKeyFunction != NULL)
	{
		entry->setKey("");
		entry->setNumberOfHashesUsed(0);
		entry->setValue(-1);
	}
	else
	{
		entries[entry->getPositionInArray()] = NULL;
		delete entry;
	}
	size--;
}

template <class K, class V>
void MonkeyHashMap<K,V>::addContributionToNumberOfHashesUsed(MonkeyHashMapEntry<K,V> *entry)
{
	int numberOfHashesUsed = entry->getNumberOfHashesUsed();
	entryCountByNumberOfHashesUsed[numberOfHashesUsed]++;
	if (numberOfHashesUsed > hashesInUse) hashesInUse = numberOfHashesUsed;
}

template <class K, class V>
void MonkeyHashMap<K,V>::removeContributionToNumberOfHashesUsed(MonkeyHashMapEntry<K,V> *entry)
{
	int numberOfHashesUsed = entry->getNumberOfHashesUsed();
	entryCountByNumberOfHashesUsed[numberOfHashesUsed]--;
	if(entryCountByNumberOfHashesUsed[numberOfHashesUsed] == 0 && hashesInUse == numberOfHashesUsed)
	{
		for(int i = numberOfHashesUsed - 1; i >= 1; i--)
			if (entryCountByNumberOfHashesUsed[i] > 0)
			{
				hashesInUse = i;
				return;
			}
		hashesInUse = 0;
	}
}

template <class K, class V>
int MonkeyHashMap<K,V>::getNextPowerOfTwo(int value)
{
	if(value <= 0) return 1;
	value--;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	value++;
	return value;
}

template <class K, class V>
void MonkeyHashMap<K,V>::print()
{
	for(int i = 0; i < arrayLength; i++)
		if(entries[i] != NULL) cout << entries[i]->getKey() << ": " << entries[i]->getValue() << "\n";
		else cout << "Null\n";
	cout << "\nNumber of functions used: " << hashesInUse << "\n";
}

template <class K, class V>
MonkeyHashMap<K,V>::~MonkeyHashMap()
{
	for(int i = 0; i < arrayLength; i++)
		if(entries[i] != NULL)
			delete entries[i];
	delete entries;
	delete entryCountByNumberOfHashesUsed;
}

template class MonkeyHashMap<string,int>;
