//
// Created by yyx on 24-1-21.
//

#ifndef SHAREHASH_UNORDEREDMAPINTERFACE_H
#define SHAREHASH_UNORDEREDMAPINTERFACE_H

#include "../MapInterface.h"
#include <unordered_map>

template<class Key, class Value>
class UnorderedMapInterface : public MapInterface<Key, Value>{
public:
    void init();

    void bulk_load(std::pair<Key, Value> *kvPairs, long long dataNumber);

    bool get(Key key, Value &val);

    bool insert(Key key, Value val);

    long long memory_consumption();

private:
    std::unordered_map<Key, Value> *index;

};

template<class Key, class Value>
void UnorderedMapInterface<Key, Value>::init() {
    index = new std::unordered_map<Key, Value>();
}

template<class Key, class Value>
void UnorderedMapInterface<Key, Value>::bulk_load(std::pair<Key, Value> *kvPairs, long long dataNumber){
    for(long long i = 0 ; i< dataNumber ; i++){
        index->insert(kvPairs[i]);
    }
}

template<class Key, class Value>
bool UnorderedMapInterface<Key, Value>::get(Key key, Value &val) {
    auto ret = index->find(key);
    if(ret != index->end()){
        val = ret->second;
        return true;
    }
    return false;
}

template<class Key, class Value>
bool UnorderedMapInterface<Key, Value>::insert(Key key, Value val) {
    (*index)[key] = val;
    return true;
}

template<class Key, class Value>
long long UnorderedMapInterface<Key, Value>::memory_consumption() {
    long long index_size = 0;
    index_size += sizeof(*index);
    return index_size;
}

#endif //SHAREHASH_UNORDEREDMAPINTERFACE_H
