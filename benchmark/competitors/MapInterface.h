//
// Created by yyx on 24-1-19.
//

#ifndef SHAREHASH_MAPINTERFACE_H
#define SHAREHASH_MAPINTERFACE_H

#include <utility>

template<class Key, class Value>
class MapInterface{
public:
    virtual void bulk_load(std::pair<Key, Value> *kvPairs, long long dataNumber);

    virtual bool get(Key key, Value &val);

    virtual bool insert(Key key, Value val);

    virtual void init();

    virtual long long memory_consumption();

};

#endif //SHAREHASH_MAPINTERFACE_H
