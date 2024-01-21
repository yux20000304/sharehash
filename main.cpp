#include <iostream>
#include "ShareHash.h"

#define DATANUMBER 10000000

int main() {
    auto *sharehashmap = new ShareHash::sharehash<int64_t, int64_t>();
    int insert_counter = 0;
    int get_counter = 0;

    for(int64_t i = 0 ; i < DATANUMBER ; i++){
        if(sharehashmap->insert(i, i))
            insert_counter++;
    }
    std::cout << "insert number:" << insert_counter << std::endl;

    for(int64_t i = 0 ; i < DATANUMBER ; i++){
        int64_t value = 0;
        if(sharehashmap->get(i, value))
            get_counter++;
        else
            std::cout << "key:" << i << " value:" << value << std::endl;
    }
    std::cout << "get number:" << get_counter << std::endl;

    return 0;
}
