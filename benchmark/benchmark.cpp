//
// Created by admin on 2024/1/18.
//

#include<iostream>
#include<chrono>
#include<unordered_map>
#include "sharehash.h"


int main(){

    std::unordered_map<int64_t , int64_t> hashmap;
    ShareHash<int64_t, int64_t> *sharehashmap = new ShareHash<int64_t, int64_t>();

    //define element number
    const int NUM_ELEMENTS = 10000000;

    //start time
    auto start = std::chrono::high_resolution_clock::now();

    //insert
    for (int64_t i = 0; i < NUM_ELEMENTS; ++i) {
        hashmap[i] = i;
    }

    //find
    for (int64_t i = 0; i < NUM_ELEMENTS; ++i) {
        auto it = hashmap.find(i);
        if (it != hashmap.end()) {
            //key exist
            int value = it->second;
        }
    }

    //stop timing
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    //print throughput
    double throughput = NUM_ELEMENTS / duration.count();
    std::cout << "Unordered map throughput: " << throughput << " elements/second" << std::endl;

    start = std::chrono::high_resolution_clock::now();

    //insert
    for (int64_t i = 0; i < NUM_ELEMENTS; ++i) {
        sharehashmap->insert(i, i);
    }

    //find
    for (int64_t i = 0; i < NUM_ELEMENTS; ++i) {
        int64_t val = 0;
        sharehashmap->get(i, val);
    }

    //stop timing
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;

    //print throughput
    throughput = NUM_ELEMENTS / duration.count();
    std::cout << "ShareHashmap throughput: " << throughput << " elements/second" << std::endl;

    return 0;
}
