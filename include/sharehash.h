//
// Created by yyx on 2024/1/2.
//

#ifndef SHAREHASH_SHAREHASH_H
#define SHAREHASH_SHAREHASH_H

#include <iostream>
#include <cstring>
#include <functional>
#include "murmur3.h"

#define SHAREBUCKETSIZE 4
#define BUCKETNUMBER 100000
#define SEARCHRANGE 500

typedef uint8_t bitmap_t;
#define BITMAP_WIDTH (sizeof(bitmap_t) * 8)
#define BITMAP_SIZE(num_items) (((num_items) + BITMAP_WIDTH - 1) / BITMAP_WIDTH)
#define BITMAP_GET(bitmap, pos) (((bitmap)[(pos) / BITMAP_WIDTH] >> ((pos) % BITMAP_WIDTH)) & 1)
#define BITMAP_SET(bitmap, pos) ((bitmap)[(pos) / BITMAP_WIDTH] |= 1 << ((pos) % BITMAP_WIDTH))
#define BITMAP_CLEAR(bitmap, pos) ((bitmap)[(pos) / BITMAP_WIDTH] &= ~bitmap_t(1 << ((pos) % BITMAP_WIDTH)))

template<typename Key, typename Value>
class ShareHash{

public:
    //top index item
    struct bucketInfo{
        int begin_index;
        int capacity;
        int counter;
    };

    //top index
    struct topIndex{
        int item_num;
        int bucket_num;
        int f_seed;
        struct bucketInfo* buckets;
        bitmap_t* none_bitmap;
    };

    //bucket item
    struct bucketItem{
        Key key;
        Value value;
    };

    bucketInfo *bucketInfo_allocator(int num){
        std::allocator<bucketInfo> alloc;
        bucketInfo *p = nullptr;
        p = alloc.allocate(num);
        return p;
    }

    void bucketInfo_deallocator(bucketInfo *p, int num){
        std::allocator<bucketInfo> dealloc;
        dealloc.deallocate(p, num);
    }

    topIndex *topIndex_allocator(int num){
        std::allocator<topIndex> alloc;
        topIndex *p = nullptr;
        p = alloc.allocate(num);
        return p;
    }

    void topIndex_deallocator(topIndex *p, int num){
        std::allocator<topIndex> dealloc;
        dealloc.deallocate(p, num);
    }

    bucketItem *bucketItem_allocator(int num){
        std::allocator<bucketItem> alloc;
        bucketItem *p = nullptr;
        p = alloc.allocate(num);
        return p;
    }

    void bucketItem_deallocator(bucketItem *p, int num){
        std::allocator<bucketItem> dealloc;
        dealloc.deallocate(p, num);
    }

    bitmap_t* bitmap_allocator(int num){
        std::allocator<bitmap_t> alloc;
        bitmap_t* p = alloc.allocate(num);
        return p;
    }

    void bitmap_deallocator(bitmap_t* p, int num){
        std::allocator<bitmap_t> dealloc;
        dealloc.deallocate(p, num);
    }

    //initialize hash table
    void initShareHash(){
        top_index = topIndex_allocator(1);
        top_index->item_num = BUCKETNUMBER * SHAREBUCKETSIZE;
        top_index->bucket_num = BUCKETNUMBER;
        top_index->buckets = bucketInfo_allocator(BUCKETNUMBER);
        top_index->f_seed = rand() % MAX_PRIME32;
        top_index->none_bitmap = bitmap_allocator(BITMAP_SIZE(top_index->item_num));
        memset(top_index->none_bitmap, 0xff, sizeof(bitmap_t) * BITMAP_SIZE(top_index->item_num));
        bucket_items = bucketItem_allocator(BUCKETNUMBER * SHAREBUCKETSIZE);

        //clear bucket data
        for(int i = 0 ; i < BUCKETNUMBER ; i++){
            top_index->buckets[i].begin_index = i * SHAREBUCKETSIZE;
            top_index->buckets[i].capacity = SHAREBUCKETSIZE;
            top_index->buckets[i].counter = 0;
        }
    }

    //delete the hash table
    void deleteShareHash(struct topIndex *delete_top_index){
        bucketItem_deallocator(bucket_items, delete_top_index->item_num);
        bucketInfo_deallocator(delete_top_index->buckets, delete_top_index->bucket_num);
        bitmap_deallocator(delete_top_index->none_bitmap, BITMAP_SIZE(delete_top_index->item_num));
        topIndex_deallocator(delete_top_index, 1);
    }

public:

    ShareHash(){
        initShareHash();
    }

    ~ShareHash(){
        deleteShareHash();
    }

    bool get(const Key &key, Value &value){
//        int bucket_num = murmurHash(reinterpret_cast<const void*>(&key), sizeof(key), top_index->f_seed) % top_index->bucket_num;
        int bucket_num = hash_func(key) % top_index->bucket_num;
        struct bucketInfo *bucket = top_index->buckets + bucket_num;
        int item_index = bucket->begin_index;
        for(int i = item_index; i < bucket->capacity + item_index; i++){
            if(!BITMAP_GET(top_index->none_bitmap, i)){
                if(bucket_items[i].key == key){
                    value = bucket_items[i].value;
                    return true;
                }
            }
        }
        return false;
    }

    bool insert(const Key &key, const Value &value){
        retry:
        //locate the bucket
//        int bucket_num = murmurHash(reinterpret_cast<const void*>(&key), sizeof(key), top_index->f_seed) % top_index->bucket_num;
        int bucket_num = hash_func(key) % top_index->bucket_num;
        struct bucketInfo *bucket = top_index->buckets + bucket_num;

        //bucket is available
        if(bucket->counter < bucket->capacity){
            BITMAP_CLEAR(top_index->none_bitmap, bucket->begin_index + bucket->counter);
            bucket_items[bucket->begin_index + bucket->counter].key = key;
            bucket_items[bucket->begin_index + bucket->counter].value = value;
            bucket->counter++;
        }
        else{   //bucket is full, we need to evict data in the next bucket
            //check the next five bucket
            int cur_bucket_num = bucket_num + 1;
            int offset = 1;
            while(cur_bucket_num < top_index->bucket_num && offset < SEARCHRANGE){
                if(checkNextBucket(cur_bucket_num)){  //have slot to evict
                    //do eviction
                    while(cur_bucket_num > bucket_num){
                        bucketInfo* cur_bucket = top_index->buckets + cur_bucket_num;
                        bucketInfo* pre_bucket = top_index->buckets + cur_bucket_num - 1;
                        int old_index = cur_bucket->begin_index;
                        int new_index = cur_bucket->begin_index;
                        //find the first occupied slot
                        for(int i = 0 ; i < cur_bucket->capacity; i++){
                            if(BITMAP_GET(top_index->none_bitmap,cur_bucket->begin_index + i)){
                                new_index += i;
                                break;
                            }
                        }
                        //find the first empty slot
                        for(int i = 0 ; i < cur_bucket->capacity; i++){
                            if(!BITMAP_GET(top_index->none_bitmap,cur_bucket->begin_index + i)){
                                old_index += i;
                                break;
                            }
                        }
                        BITMAP_CLEAR(top_index->none_bitmap, new_index);
                        BITMAP_SET(top_index->none_bitmap, old_index);
                        bucket_items[new_index].key = bucket_items[old_index].key;
                        bucket_items[new_index].value = bucket_items[old_index].value;
                        cur_bucket->begin_index++;
                        cur_bucket->capacity--;
                        pre_bucket->capacity++;
                        cur_bucket_num--;
                    }
                    break;
                }
                offset++;
                cur_bucket_num++;
            }

            if(offset == SEARCHRANGE || cur_bucket_num == top_index->bucket_num){    //not enough space
                //do resize
                resize();
                goto retry;
            }
            BITMAP_CLEAR(top_index->none_bitmap, bucket->begin_index + bucket->counter);
            bucket_items[bucket->begin_index + bucket->counter].key = key;
            bucket_items[bucket->begin_index + bucket->counter].value = value;
            bucket->counter++;
        }

        return true;
    }

    bool remove(const Key &key){
        int bucket_num = murmurHash(reinterpret_cast<const void*>(&key), sizeof(key), top_index->f_seed) % top_index->bucket_num;
        struct bucketInfo *bucket = top_index->buckets + bucket_num;
        int item_index = bucket->begin_index;
        for(int i = item_index; i < bucket->capacity; i++){
            if(!BITMAP_GET(top_index->none_bitmap, i)){
                if(bucket_items[i].key == key){
                    BITMAP_SET(top_index->none_bitmap, i);
                    return true;
                }
            }
        }
        return false;
    }

    bool resize(){
        //simply rehash all the elements to the new array
        struct topIndex *old_top_index = top_index;
        struct bucketItem *old_bucket_items = bucket_items;
        top_index = topIndex_allocator(1);
        top_index->bucket_num = old_top_index->bucket_num * 2;
        top_index->item_num = old_top_index->item_num * 2;
        top_index->f_seed = old_top_index->f_seed;
        top_index->buckets = bucketInfo_allocator(top_index->bucket_num);
        bucket_items = bucketItem_allocator(top_index->item_num);
        top_index->none_bitmap = bitmap_allocator(top_index->item_num);
        memset(top_index->none_bitmap, 0xff, sizeof(bitmap_t) * BITMAP_SIZE(top_index->item_num));

        int bucket_num = 0;

        for(int i = 0 ; i < top_index->bucket_num ; i++){
            top_index->buckets[i].begin_index = i * SHAREBUCKETSIZE;
            top_index->buckets[i].capacity = SHAREBUCKETSIZE;
            top_index->buckets[i].counter = 0;
        }

        //copy data
        for(int i = 0 ; i < old_top_index->item_num ; i++){
            if(!BITMAP_GET(old_top_index->none_bitmap, i)){
                insert(old_bucket_items[i].key, old_bucket_items[i].value);
            }
        }
        //destroy old data
        bucketItem_deallocator(old_bucket_items, old_top_index->item_num);
        bucketInfo_deallocator(old_top_index->buckets, old_top_index->bucket_num);
        bitmap_deallocator(old_top_index->none_bitmap, BITMAP_SIZE(old_top_index->item_num));
        topIndex_deallocator(old_top_index, 1);

        std::cout << "resize done!" << std::endl;
        return true;
    }

    bool checkNextBucket(int &bucket_num){
        bucketInfo *bucket;
        bucket = top_index->buckets + bucket_num;
        return bucket->counter < bucket->capacity;
    }

private:
    struct topIndex *top_index;
    struct bucketItem *bucket_items;
    std::hash<Key> hash_func;
};


#endif //SHAREHASH_SHAREHASH_H
