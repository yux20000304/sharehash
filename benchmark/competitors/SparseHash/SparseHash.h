//
// Created by admin on 2024/1/24.
//

#ifndef SHAREHASH_SPARSEHASH_H
#define SHAREHASH_SPARSEHASH_H

#include <google/sparse_hash_map>
#include "../MapInterface.h"

template<class Key, class Value>
class SparseHashInterface : public MapInterface<Key, Value>{
public:
    void init();

    void bulk_load(std::pair<Key, Value> *kvPairs, long long dataNumber);

    bool get(Key key, Value &val);

    bool insert(Key key, Value val);

    long long memory_consumption();

private:
    google::sparse_hash_map<Key, Value> *index;

};

template<class Key, class Value>
void SparseHashInterface<Key, Value>::init() {
    index = new google::sparse_hash_map<Key, Value>();
}

template<class Key, class Value>
void SparseHashInterface<Key, Value>::bulk_load(std::pair<Key, Value> *kvPairs, long long dataNumber){
    for(int i = 0 ; i < dataNumber; i++){
        index->insert(kvPairs[i]);
    }
}

template<class Key, class Value>
bool SparseHashInterface<Key, Value>::get(Key key, Value &val) {
    auto ret = index->find(key);
    if(ret != index->end()){
        val = ret->second;
        return true;
    }
    return false;
}

template<class Key, class Value>
bool SparseHashInterface<Key, Value>::insert(Key key, Value val) {
    (*index)[key] = val;
    return true;
}

template<class Key, class Value>
long long SparseHashInterface<Key, Value>::memory_consumption() {
    return index->size() * (sizeof(Key) + sizeof(Value)) / index->load_factor();
}

#endif //SHAREHASH_SPARSEHASH_H
