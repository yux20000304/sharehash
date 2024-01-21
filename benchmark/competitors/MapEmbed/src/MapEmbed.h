#ifndef MapEmbed_H
#define MapEmbed_H

//This the implementation of MapEmbed Hashing in C++.
#include "immintrin.h"
#include "./murmur.h"
#include <cstring>
#include <random>
#include <set>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

namespace MapEmbed{

    class MapEmbed{
        // begin kv pair
    public:
        struct KV_entry{
            char key[8];
            char value[8];
        };

        KV_entry kvPairs[30000000+10];

        inline void random_string(char* str, int len){
            for(int i = 0; i < len; ++i)
                str[i] = '!' + rand()%93;
            str[len-1] = 0;
        }

        void create_random_kvs(KV_entry *kvs, int num){
            srand((uint32_t)time(0));
            for(int i = 0; i < num; ++i){
                random_string(kvs[i].key, 8);
                random_string(kvs[i].value, 8);
            }
        }

        void create_random_kvs_keyint(KV_entry *kvs, int num){
            for(uint32_t i = 0; i < num; ++i){
                *(int*)kvs[i].key = i;
                *(int*)kvs[i].value = i;
            }
        }

        // end kv pair


        int cnt = 0; // debug use

//    #define MAX_LAYER 12
//    #define M 32                // max number of hash functions in each layer
//    #define N 16                 // item number in a bucket

        struct Bucket{
            char key[16][8];
            char value[16][8];
            int layer[16];           // item's layer
            int k[16];               // item's cell index
            int used;               // used slots
            bool check;
            char pad[27];
        };
    public:
        int bucket_number;

        int cell_layer;
        int cell_number[12];     // cell number of each layer

        int cell_bit;        // bit number of each cell
        int cell_hash;       // number of hash functions in each layer, 2^(cell's bit number)
        int cell_full;       // 2^(cell's bit number)-1

        int cell_offset[12];     // used to calculate the unique ID of a cell, e.g. cell[l][k]'s ID is k+cell_offset[l]

    #ifdef USING_SIMD
        __attribute__((aligned(32))) Bucket *bucket;
    #else
        Bucket *bucket;
    #endif
        uint32_t **cell;

        uint32_t seed_hash_to_cell[12];
        uint32_t seed_hash_to_bucket[32];
        uint32_t seed_hash_to_guide;

    // below are print status
    public:
        int bucket_items;

    private:
        void initialize_hash_functions(){
            std::set<int> seeds;
            uint32_t seed = rand()%MAX_PRIME32;
            seed_hash_to_guide = seed;
            seeds.insert(seed);
            for(int i = 0; i < cell_layer; ++i){
                seed = rand()%MAX_PRIME32;
                while(seeds.find(seed) != seeds.end())
                    seed = rand()%MAX_PRIME32;
                seed_hash_to_cell[i] = seed;
                seeds.insert(seed);
            }
            for(int i = 0; i < 32; ++i){
                seed = rand()%MAX_PRIME32;
                while(seeds.find(seed) != seeds.end())
                    seed = rand()%MAX_PRIME32;
                seed_hash_to_bucket[i] = seed;
                seeds.insert(seed);
            }
        }


        int calculate_cell(const char* key, int layer){
            // cost: calculate 1 hash
            uint64_t hash_ret = 0;
            murmurHash(key, 8, seed_hash_to_cell[layer], &hash_ret);
            return  hash_ret % cell_number[layer];
        }

        int calculate_guide(const char* key, int cell_value){
            // cost: calculate 1 hash
            uint64_t tmp = 0;
            murmurHash(key, 8, seed_hash_to_guide, &tmp);
            int ret = ((tmp & (cell_hash - 1)) + cell_value) % cell_hash;
            return ret;
        }

        int calculate_bucket(const char* key, int layer, int k){
            // cost: calculate 2 hash, plus visit cell[layer][k]
            int guide = calculate_guide(key, cell[layer][k]);
            int cell_ID = k + cell_offset[layer];
            int page_size = bucket_number / cell_hash;
            int ret = 0;
            murmurHash((const char*)&cell_ID, sizeof(int), seed_hash_to_bucket[guide], &ret);
            return ret % page_size + guide * page_size;
        }

        int calculate_bucket_from_cell(int layer, int guide, int cell_ID){
            // cost: calculate 1 hash
            int page_size = bucket_number / cell_hash;
            int ret = 0;
            murmurHash((const char*)&cell_ID, sizeof(int), seed_hash_to_bucket[guide], &ret);
            return ret % page_size + guide * page_size;
        }


        bool insert_kv_to_bucket(const KV_entry& kv, int d, int layer, int k){
            if(bucket[d].check == false)
                check_bucket(d);
            int used = bucket[d].used;
            if(used >= 16)
                // bucket full
                return false;
            memcpy(bucket[d].key[used], kv.key, (8)*sizeof(char));
            memcpy(bucket[d].value[used], kv.value, (8)*sizeof(char));
            bucket[d].layer[used] = layer;
            bucket[d].k[used] = k;
            bucket[d].used++;
            return true;
        }

        // the returned bitmap size restrict M <= 32
        inline int collect_kv(int k, int layer, KV_entry *kvs, int *cell_bucket, int *bucket_empty, int &size){
            int ret = 0;
            // return bitmap, 1: full 0: not full
            int cell_ID = k + cell_offset[layer];
            for(int guide = 0; guide < cell_hash; ++guide){
                int d = calculate_bucket_from_cell(layer, guide, cell_ID);
                cell_bucket[guide] = d;
                for(int j = 0; j < bucket[d].used; ++j){
                    if(bucket[d].layer[j] == layer && bucket[d].k[j] == k){
                        memcpy(kvs[size].key, bucket[d].key[j], (8)*sizeof(char));
                        memcpy(kvs[size].value, bucket[d].value[j], (8)*sizeof(char));
                        size++;
                        int used = bucket[d].used;
                        if(j == used - 1)
                            bucket[d].used--;
                        else{
                            memcpy(bucket[d].key[j], bucket[d].key[used-1], (8)*sizeof(char));
                            memcpy(bucket[d].value[j], bucket[d].value[used-1], (8)*sizeof(char));
                            bucket[d].layer[j] = bucket[d].layer[used-1];
                            bucket[d].k[j] = bucket[d].k[used-1];
                            bucket[d].used--;
                            j--;
                        }
                    }
                }
                if(bucket[d].used >= 16)
                    ret |= (1 << guide);
                bucket_empty[guide] = 16 - bucket[d].used;
            }
            return ret;
        }

        inline bool check_kvs(int now_cell, int *bucket_empty, int *kvs_guide_bucket_count){
            int hash_number = cell_hash;
            for(int i = 0; i < hash_number; ++i){
                int now_guide = (i + hash_number - now_cell) % hash_number;
                if(kvs_guide_bucket_count[now_guide] > bucket_empty[i])
                    return false;
            }
            return true;
        }

        inline void dispense_kvs(const KV_entry *kvs, int size, int k, int layer, int *cell_bucket, int *kvs_now_guide){
            for(int i = 0; i < size; ++i){
                int guide = kvs_now_guide[i];
                int d = cell_bucket[guide];
                insert_kv_to_bucket(kvs[i], d, layer, k);
            }
        }

        void calculate_kvs_initial_guide(const KV_entry *kvs, int size, int *kvs_initial_guide, int *kvs_guide_bucket_count){
            for(int i = 0; i < size; ++i){
                int guide = calculate_guide(kvs[i].key, 0);
                kvs_initial_guide[i] = guide;
                kvs_guide_bucket_count[guide]++;
            }
        }

        void calculate_kvs_now_guide(int *kvs_initial_guide, int *kvs_now_guide, int size, int now_cell){
            for(int i = 0; i < size; ++i)
                kvs_now_guide[i] = (kvs_initial_guide[i] + now_cell) % cell_hash;
        }

        int calculate_kvs_initial_bitmap(int layer, int size, int *kvs_initial_guide){
            int ret = 0;
            for(int i = 0; i < size; ++i){
                int guide = kvs_initial_guide[i];
                ret |= (1 << guide);
            }
            return ret;
        }

        inline int shift_kvs_bitmap(int kvs_initial_bitmap, int now_cell){
            int hash_number = cell_hash;
            int tmp_mask = ((1 << now_cell) - 1);
            int full_mask = ((1 << hash_number) - 1);
            int tmp = (((kvs_initial_bitmap << now_cell) & (tmp_mask << hash_number)) >> hash_number);
            int ret = ((kvs_initial_bitmap << now_cell) | tmp) & full_mask;
            return ret;
        }

        bool query_key_in_bucket(const char* key, int d, char* value = NULL){
            if(bucket[d].check == false)
                check_bucket(d);

    #ifdef USING_SIMD
            const __m256i item = _mm256_set1_epi32(*(int*)key);
            __m256i *keys_p = (__m256i *)(bucket[d].key);
            int matched = 0;

            __m256i a_comp = _mm256_cmpeq_epi32(item, keys_p[0]);
            matched = _mm256_movemask_ps((__m256)a_comp);


            if(matched != 0){
                int matched_lowbit = matched & (-matched);
                int matched_index = _mm_tzcnt_32((uint32_t)matched_lowbit);
                if(matched_index < bucket[d].used){
                    if(value != NULL){
                        memcpy(value, bucket[d].value[matched_index], 8*sizeof(char));
                    }
                    return true;
                }
                return false;
            }
            return false;

    #else
           int used = bucket[d].used;
           for(int i = 0; i < used; ++i){
               if(memcmp(bucket[d].key[i], key, 8*sizeof(char)) == 0){
                   if(value != NULL)
                       memcpy(value, bucket[d].value[i], 8*sizeof(char));
                   return true;
               }
           }
           return false;
    #endif
        }

        int query_bucket(const char *key){
            for(int layer = 0; layer < cell_layer; ++layer){
                int k = calculate_cell(key, layer);
                if(cell[layer][k] == cell_full)
                    continue;
                int d = calculate_bucket(key, layer, k);
                return d;
            }
            return -1;
        }

        void check_bucket(int d){
            int used = bucket[d].used;
            for(int i = 0; i < used; ++i){
                int dd = query_bucket(bucket[d].key[i]);
                if(dd != d){
                    used--;
                    memcpy(bucket[d].key[i], bucket[d].key[used], 8);
                    memcpy(bucket[d].value[i], bucket[d].value[used], 8);
                    bucket[d].layer[i] = bucket[d].layer[used];
                    bucket[d].k[i] = bucket[d].k[used];
                    i--;
                }
            }
            bucket[d].used = used;
            bucket[d].check = true;
        }

    public:
        MapEmbed(int layer_, int bucket_number_, int* cell_number_, int cell_bit_){
            cell_layer = layer_;
            bucket_number = bucket_number_;
            bucket = new Bucket[bucket_number];
            for(int i = 0; i < bucket_number; ++i){
                bucket[i].used = false;
                bucket[i].check = true;
            }

            for(int i = 0; i < cell_layer; ++i)
                cell_number[i] = cell_number_[i];
            cell_bit = cell_bit_;
            cell_hash = pow(2, cell_bit);
            cell_full = cell_hash - 1;

            cell = new uint32_t* [cell_layer];
            for(int i = 0; i < cell_layer; ++i){
                cell[i] = new uint32_t[cell_number[i] + 10];
                memset(cell[i], 0, (cell_number[i]+10)*sizeof(uint32_t));
            }
            cell_offset[0] = 0;
            for(int i = 1; i < cell_layer; ++i)
                cell_offset[i] = cell_offset[i-1] + cell_number[i-1];
            initialize_hash_functions();

            bucket_items = 0;
        }

        ~MapEmbed(){
            for(int i = 0; i < cell_layer; ++i)
                delete [] cell[i];
            delete [] cell;
            delete [] bucket;
        }

        bool insert(const KV_entry& kv, int start_layer = 0){
            // try to insert 'kv' from 'start_layer'
            if(start_layer >= cell_layer){
                cnt++; //debug
                return false;
            }
            for(int layer = start_layer; layer < cell_layer; ++layer){
                int k = calculate_cell(kv.key, layer);
                // k: cell index
                if(cell[layer][k] == cell_full)
                    continue;
                int d = calculate_bucket(kv.key, layer, k);
                // d: bucket index
                if(insert_kv_to_bucket(kv, d, layer, k))
                    return true;
                // directly insert failed, collect items and adjust cell state
                KV_entry kvs[32*16+5];
                int cell_bucket[32+5] = {};
                int bucket_empty[32+5] = {};

                int size = 0;
                memcpy(kvs[size].key, kv.key, (8)*sizeof(char));
                memcpy(kvs[size].value, kv.value, (8)*sizeof(char));
                size++;
                int cell_bitmap = collect_kv(k, layer, kvs, cell_bucket, bucket_empty, size);
                // full: 1, not full: 0

                int kvs_initial_guide[32*16+5] = {};
                int kvs_now_guide[32*16+5] = {};
                int kvs_guide_bucket_count[32+5] = {};
                calculate_kvs_initial_guide(kvs, size, kvs_initial_guide, kvs_guide_bucket_count);
                int kvs_initial_bitmap = calculate_kvs_initial_bitmap(layer, size, kvs_initial_guide);

                int now_cell = cell[layer][k];

                while(true){
                    now_cell++;
                    if(now_cell == cell_full){
                        cell[layer][k] = now_cell;
                        bool ret = true;
                        for(int i = 0; i < size; ++i)
                            ret &= insert(kvs[i], layer+1);
                        return ret;
                    }
                    int kvs_bitmap = shift_kvs_bitmap(kvs_initial_bitmap, now_cell);
                    if((kvs_bitmap & cell_bitmap) != 0)
                        continue;
                    if(check_kvs(now_cell, bucket_empty, kvs_guide_bucket_count)){
                        cell[layer][k] = now_cell;
                        calculate_kvs_now_guide(kvs_initial_guide, kvs_now_guide, size, now_cell);
                        dispense_kvs(kvs, size, k, layer, cell_bucket, kvs_now_guide);
                        return true;
                    }
                }
            }
            return false;
        }

        bool query(const char *key, char* value = NULL){
            for(int layer = 0; layer < cell_layer; ++layer){
                int k = calculate_cell(key, layer);
                if(cell[layer][k] == cell_full)
                    continue;
                int d = calculate_bucket(key, layer, k);
                return query_key_in_bucket(key, d, value);
            }
            return false;
        }

        void extend(bool lazy = true){  //note! initial bucket number must be multiple of `cell_hash'
            Bucket* bucket_new = new Bucket[bucket_number*2];
            int page_size = bucket_number / cell_hash;
            for(int i = bucket_number - 1; i >= 0; --i){
                int p = i / page_size;
                int q = i % page_size;
                // copy i to 2*p*page_size+q and 2*p*page_size+page_size+q
                int j1 = 2*p*page_size + q;
                int j2 = 2*p*page_size + page_size + q;
                memcpy(bucket_new + j1, bucket + i, sizeof(Bucket));
                memcpy(bucket_new + j2, bucket + i, sizeof(Bucket));
                bucket_new[j1].check = bucket_new[j2].check = false;
            }
            delete [] bucket;
            bucket = bucket_new;
            bucket_number *= 2;

            if(lazy == false){
                for(int i = 0; i < bucket_number; ++i)
                    if(bucket[i].check == 0)
                        check_bucket(i);
            }
        }

    // below are print status functions
    public:
        int calculate_bucket_items(){
            bucket_items = 0;
            for(int i = 0; i < bucket_number; ++i){
                if(bucket[i].check == 0)
                    check_bucket(i);
                bucket_items += bucket[i].used;
            }
            return bucket_items;
        }

        double bit_per_item(){
            calculate_bucket_items();
            int cell_bit_sum = 0;
            for(int i = 0; i < cell_layer; ++i)
                cell_bit_sum += cell_number[i] * cell_bit;
            return (double)cell_bit_sum / bucket_items;
        }

        double load_factor(){
            calculate_bucket_items();
            int bucket_slots = bucket_number * 16;
            return (double)bucket_items / bucket_slots;
        }
    };

}


#endif
