#ifndef MONKEY_HASH_MAP_ENTRY
#define MONKEY_HASH_MAP_ENTRY

template <class K, class V>
class MonkeyHashMapEntry
{
private:
	K key;
	V value;
	int numberOfHashesUsed;
	int positionInArray;
public:
	MonkeyHashMapEntry(K key, V value, int positionInArray);
	V getValue();
	V setValue(V value);
	K getKey();
	void setKey(K);
	int getNumberOfHashesUsed();
	void setNumberOfHashesUsed(int);
	int getPositionInArray();
};

#endif
