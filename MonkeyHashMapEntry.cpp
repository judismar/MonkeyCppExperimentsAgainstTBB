#include "MonkeyHashMapEntry.hpp"
#include <iostream>

template <class K, class V>
MonkeyHashMapEntry<K,V>::MonkeyHashMapEntry(K key, V value, int positionInArray)
{
	this->key = key;
	this->value = value;
	this->positionInArray = positionInArray;
}

template <class K, class V>
V MonkeyHashMapEntry<K,V>::getValue()
{
	return value;
}

template <class K, class V>
V MonkeyHashMapEntry<K,V>::setValue(V value)
{
	V oldValue = this->value;
	this->value = value;
	return oldValue;
}

template <class K, class V>
K MonkeyHashMapEntry<K,V>::getKey()
{
	return key;
}

template <class K, class V>
void MonkeyHashMapEntry<K,V>::setKey(K key)
{
	this->key = key;
}

template <class K, class V>
int MonkeyHashMapEntry<K,V>::getNumberOfHashesUsed()
{
	return numberOfHashesUsed;
}

template <class K, class V>
void MonkeyHashMapEntry<K,V>::setNumberOfHashesUsed(int v)
{
	numberOfHashesUsed = v;
}

template <class K, class V>
int MonkeyHashMapEntry<K,V>::getPositionInArray()
{
	return positionInArray;
}

template class MonkeyHashMapEntry<std::string,int>;
