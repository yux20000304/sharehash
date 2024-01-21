//
// Created by yyx on 2024/1/18.
//

#include<iostream>
#include<chrono>
#include <map>
#include <boost/program_options.hpp>
#include <random>
#include <vector>
#include "./competitors/MapInterface.h"
#include "./competitors/MapEmbed/MapEmbedInterface.h"
#include "./competitors/ShareHash/ShareHashInterface.h"
#include "./competitors/UnorderedMap/UnorderedMapInterface.h"

int main(int argc, char** argv){

    //benchmark configuration
    int datasetNumber = 0;
    MapInterface<uint64_t, uint64_t > *hashmap;

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
            ("hashmap-type,t", boost::program_options::value<std::string>(), "Input hashmap type")
            ("dataset-size,d", boost::program_options::value<std::string>(), "Dataset size")
            ("help", "Print help message");

    boost::program_options::variables_map vm;
    try {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);
    } catch (const boost::program_options::error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    if (vm.count("hashmap-type")) {
        std::string hashmapType = vm["hashmap-type"].as<std::string>();
        std::cout << "Hashmap type: " << hashmapType << std::endl;

        if(hashmapType == "unordered_map"){
            hashmap = new UnorderedMapInterface<uint64_t, uint64_t>;
        }
        else if(hashmapType == "mapembed"){
            hashmap = new MapEmbedInterface<uint64_t , uint64_t>;
        }
        else if(hashmapType == "sharehash"){
            hashmap = new ShareHashInterface<uint64_t, uint64_t >;
        }
    }

    if (vm.count("dataset-size")) {
        std::string datasetSize = vm["dataset-size"].as<std::string>();
        std::cout << "Dataset size: " << datasetSize << std::endl;

        datasetNumber = std::stoi(datasetSize);
    }

    hashmap->init();

    //generate data
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int64_t> dis(std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());
    std::vector<std::pair<int64_t, int64_t>> dataset;

    dataset.reserve(datasetNumber);

    for(int i = 0 ; i < datasetNumber ; i++) {
        int64_t randomNum = dis(gen);
        dataset[i] = {randomNum, randomNum};
    }

    //start time
    auto start = std::chrono::high_resolution_clock::now();

    //insert
    for(int i = 0 ; i < datasetNumber ; i++){
        hashmap->insert(dataset[i].first, dataset[i].second);
    }

    //stop timing
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    //print throughput
    double throughput = datasetNumber / duration.count();
    std::cout << "Hashmap insert throughput: " << throughput << " elements/second" << std::endl;

    uint64_t get_val = 0;
    start = std::chrono::high_resolution_clock::now();

    //read
    for(int i = 0 ; i < datasetNumber ; i++){
//        hashmap->get(dataset[i].first, get_val);
    }

    end = std::chrono::high_resolution_clock::now();
    duration = end - start;

    //print throughput
    throughput = datasetNumber / duration.count();
    std::cout << "Hashmap read throughput: " << throughput << " elements/second" << std::endl;

    //memory consumption
    std::cout << "Space consumption: " << hashmap->memory_consumption() << "bytes" << std:: endl;

    return 0;
}
