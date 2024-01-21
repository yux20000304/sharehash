//
// Created by yyx on 24-1-19.
//

#ifndef SHAREHASH_SHAREHASHINTERFACE_H
#define SHAREHASH_SHAREHASHINTERFACE_H

#include "ShareHash.h"
#include "../MapInterface.h"

template<class Key, class Value>
class ShareHashInterface : public MapInterface<Key, Value>{
public:
    void init();

    void bulk_load(std::pair<Key, Value> *kvPairs, long long dataNumber);

    bool get(Key key, Value &val);

    bool insert(Key key, Value val);

    long long memory_consumption();

private:
    ShareHash::sharehash<Key, Value> *index;

};

template<class Key, class Value>
void ShareHashInterface<Key, Value>::init() {
    index = new ShareHash::sharehash<Key, Value>();
}

template<class Key, class Value>
void ShareHashInterface<Key, Value>::bulk_load(std::pair<Key, Value> *kvPairs, long long dataNumber){
    for(long long i = 0 ; i< dataNumber ; i++){
        index->insert(kvPairs[i].first, kvPairs[i].second);
    }
}

template<class Key, class Value>
bool ShareHashInterface<Key, Value>::get(Key key, Value &val) {
    return index->get(key, val);
}

template<class Key, class Value>
bool ShareHashInterface<Key, Value>::insert(Key key, Value val) {
    return index->insert(key, val);
}

template<class Key, class Value>
long long ShareHashInterface<Key, Value>::memory_consumption() {
    return index->memoryUsage();
}


#endif //SHAREHASH_SHAREHASHINTERFACE_H
