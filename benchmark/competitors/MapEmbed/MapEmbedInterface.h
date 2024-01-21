//
// Created by yyx on 24-1-19.
//

#ifndef SHAREHASH_MAPEMBEDINTERFACE_H
#define SHAREHASH_MAPEMBEDINTERFACE_H

#include "./src/MapEmbed.h"
#include "../MapInterface.h"

template<class Key, class Value>
class MapEmbedInterface : public MapInterface<Key, Value>{
public:
    void init();

    void bulk_load(std::pair<Key, Value> *kvPairs, long long dataNumber);

    bool get(Key key, Value &val);

    bool insert(Key key, Value val);

    long long memory_consumption();

private:
    MapEmbed::MapEmbed *index;

};

template<class Key, class Value>
void MapEmbedInterface<Key, Value>::init() {
    int layer = 3;
    int bucket_number = 500000;
    int cell_number[3];
    cell_number[0] = 2250000;
    cell_number[1] = 750000;
    cell_number[2] = 250000;
    int cell_bit = 4;

    index = new MapEmbed::MapEmbed(layer, bucket_number, cell_number, cell_bit);
};

template<class Key, class Value>
void MapEmbedInterface<Key, Value>::bulk_load(std::pair<Key, Value> *kvPairs, long long dataNumber){
    MapEmbed::MapEmbed::KV_entry kv_pair{0,0};
    for(long long i = 0 ; i < dataNumber ; i++){
        *(Key*)kv_pair.key = kvPairs[i].first;
        *(Value*)kv_pair.value = kvPairs[i].second;
        index->insert(kv_pair);
    }
}

template<class Key, class Value>
bool MapEmbedInterface<Key, Value>::get(Key key, Value &val) {
    const char *key_ptr = reinterpret_cast<const char*>(&key);
    return index->query(key_ptr);
}

template<class Key, class Value>
bool MapEmbedInterface<Key, Value>::insert(Key key, Value val) {
    MapEmbed::MapEmbed::KV_entry kv_pair{};
    *(Key*)kv_pair.key = key;
    *(Value*)kv_pair.value = val;
    bool ret = index->insert(kv_pair);
    return ret;
}

template<class Key, class Value>
long long MapEmbedInterface<Key, Value>::memory_consumption() {
    long long index_size = sizeof(*index);
    index_size += index->bucket_number * sizeof(MapEmbed::MapEmbed::Bucket);

    index_size += index->cell_layer * sizeof(uint32_t);
    for(int i = 0; i < index->cell_layer; ++i){
        index_size += (index->cell_number[i] + 10) * sizeof(uint32_t);
    }

    return index_size;
}

#endif //SHAREHASH_MAPEMBEDINTERFACE_H
