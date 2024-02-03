//
// Created by admin on 2024/1/24.
//

#ifndef SHAREHASH_CUCKOOMAPINTERFACE_H
#define SHAREHASH_CUCKOOMAPINTERFACE_H

#include <libcuckoo/cuckoohash_map.hh>
#include "../MapInterface.h"

template<class Key, class Value>
class CuckooInterface : public MapInterface<Key, Value>{
public:
    void init();

    void bulk_load(std::pair<Key, Value> *kvPairs, long long dataNumber);

    bool get(Key key, Value &val);

    bool insert(Key key, Value val);

    long long memory_consumption();

private:
    libcuckoo::cuckoohash_map<Key, Value> *index;

};

template<class Key, class Value>
void CuckooInterface<Key, Value>::init() {
    index = new libcuckoo::cuckoohash_map<Key, Value>();
}

template<class Key, class Value>
void CuckooInterface<Key, Value>::bulk_load(std::pair<Key, Value> *kvPairs, long long dataNumber){
    for(int i = 0 ; i < dataNumber; i++){
        index->insert(kvPairs[i].first, kvPairs[i].second);
    }
}

template<class Key, class Value>
bool CuckooInterface<Key, Value>::get(Key key, Value &val) {
    return index->find(key, val);
}

template<class Key, class Value>
bool CuckooInterface<Key, Value>::insert(Key key, Value val) {
    return index->insert(key, val);
}

template<class Key, class Value>
long long CuckooInterface<Key, Value>::memory_consumption() {
    return index->capacity() * (sizeof(Key) + sizeof(Value));
}


#endif //SHAREHASH_CUCKOOMAPINTERFACE_H
