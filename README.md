# sharehash

## Description


## Dependencies

```
sudo apt update
sudo apt install build-essential libboost-all-dev libsparsehash-dev
```
We make and install cuckoo hashmap from [libcuckoo](https://github.com/efficient/libcuckoo)

## Build and Run

```
mkdir build && cd build
cmake ..
make -j8
./benchmark -t $(hashmap_type) -d $(dataset_number)
```

Our benchmark now support the following hashmaps:
1. unordered_map
2. sharehash
3. mapembed