//
// Created by yyx on 2024/1/18.
//

#include <iostream>
#include <chrono>
#include <map>
#include <boost/program_options.hpp>
#include <random>
#include <vector>
#include "omp.h"
#include "./competitors/MapInterface.h"
#include "./competitors/MapEmbed/MapEmbedInterface.h"
#include "./competitors/ShareHash/ShareHashInterface.h"
#include "./competitors/UnorderedMap/UnorderedMapInterface.h"
#include "./competitors/Cuckoo/CuckooMapInterface.h"
#include "./competitors/SparseHash/SparseHash.h"

struct configuration{
    int datasetNumber;
    MapInterface<uint64_t, uint64_t > *hashmap;
    int initRatio;
    double readRatio;
    double writeRatio;
    int threadNumber;
};

int main(int argc, char** argv) {

    //benchmark configuration
    struct configuration config;
    config.datasetNumber = 0;
    config.hashmap = nullptr;
    config.initRatio = 0.0;
    config.readRatio = 1.0;
    config.writeRatio = 0.0;
    config.threadNumber = 1;
    std::vector<std::thread> threads;

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
            ("hashmap-type,t", boost::program_options::value<std::string>(), "Input hashmap type")
            ("dataset-size,d", boost::program_options::value<std::string>(), "Dataset size")
            ("init-ratio,i", boost::program_options::value<std::string>(), "Init ratio")
            ("read-ratio,r", boost::program_options::value<std::string>(), "Read ratio")
            ("thread-number,p", boost::program_options::value<std::string>(), "Thread number")
            ("help", "Print help message");

    boost::program_options::variables_map vm;
    try {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);
    } catch (const boost::program_options::error &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    if (vm.count("hashmap-type")) {
        std::string hashmapType = vm["hashmap-type"].as<std::string>();
        std::cout << "Hashmap type: " << hashmapType << std::endl;

        if (hashmapType == "unorderedmap") {
            config.hashmap = new UnorderedMapInterface<uint64_t, uint64_t>;
        } else if (hashmapType == "mapembed") {
            config.hashmap = new MapEmbedInterface<uint64_t, uint64_t>;
        } else if (hashmapType == "sharehash") {
            config.hashmap = new ShareHashInterface<uint64_t, uint64_t>;
        } else if (hashmapType == "cuckoohash") {
            config.hashmap = new CuckooInterface<uint64_t, uint64_t>;
        } else if (hashmapType == "sparsehash") {
            config.hashmap = new SparseHashInterface<uint64_t, uint64_t>;
        }
    }

    if (vm.count("dataset-size")) {
        std::string datasetSize = vm["dataset-size"].as<std::string>();
        std::cout << "Dataset size: " << datasetSize << std::endl;
        config.datasetNumber = std::stoi(datasetSize);
    }

    if (vm.count("init-ratio")) {
        std::string initRatio = vm["init-ratio"].as<std::string>();
        std::cout << "Init ratio: " << initRatio << std::endl;
        config.initRatio = std::stod(initRatio);
    }

    if (vm.count("read-ratio")) {
        std::string readRatio = vm["read-ratio"].as<std::string>();
        std::cout << "Read ratio: " << readRatio << std::endl;
        config.readRatio = std::stod(readRatio);
        config.writeRatio = 1.0 - config.readRatio;
    }

    if (vm.count("thread-number")) {
        std::string threadNumber = vm["thread-number"].as<std::string>();
        std::cout << "Thread number: " << threadNumber << std::endl;
        config.threadNumber = std::stoi(threadNumber);
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    //init index
    config.hashmap->init();

    //generate data
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int64_t> dis(std::numeric_limits<int64_t>::min(),
                                               std::numeric_limits<int64_t>::max());
    std::vector<std::pair<int64_t, int64_t>> dataset;

    dataset.reserve(config.datasetNumber);
    //generate dataset
    for (int i = 0; i < config.datasetNumber; i++) {
        int64_t randomNum = dis(gen);
        dataset[i] = {randomNum, randomNum};
    }

    //start time
    auto start = std::chrono::high_resolution_clock::now();


#pragma omp parallel num_threads(config.threadNumber)
    {
        //thread info
        auto thread_id = omp_get_thread_num();
//        std::cout << thread_id << std::endl;
        int range = config.datasetNumber / config.threadNumber;
        int start = range * thread_id;
        int end = start + range;

        for (int i = start; i < end; i++) {
            config.hashmap->insert(dataset[i].first, dataset[i].second);
        }

    }
//    //insert
//    for(int i = 0 ; i < config.datasetNumber ; i++){
//        config.hashmap->insert(dataset[i].first, dataset[i].second);
//    }
    //stop timing
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    //print throughput
    long double throughput = config.datasetNumber / duration.count();
    std::cout << "Hashmap insert throughput: " << throughput << " elements/second" << std::endl;

    start = std::chrono::high_resolution_clock::now();

#pragma omp parallel num_threads(config.threadNumber)
    {
        //thread info
        auto thread_id = omp_get_thread_num();
//        std::cout << thread_id << std::endl;
        int range = config.datasetNumber / config.threadNumber;
        int start = range * thread_id;
        int end = start + range;
        uint64_t get_val = 0;

        for (int i = start; i < end; i++) {
            config.hashmap->get(dataset[i].first, get_val);
        }
        
    }

    end = std::chrono::high_resolution_clock::now();
    duration = end - start;

    //print throughput
    throughput = config.datasetNumber / duration.count();
    std::cout << "Hashmap read throughput: " << throughput << " elements/second" << std::endl;

    //memory consumption
    std::cout << "Space consumption: " << config.hashmap->memory_consumption() << "bytes" << std:: endl;

    return 0;
}
