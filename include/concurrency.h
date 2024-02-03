//
// Created by admin on 2024/1/26.
//

#ifndef SHAREHASH_CONCURRENCY_H
#define SHAREHASH_CONCURRENCY_H

#include <atomic>
#include <boost/thread.hpp>

namespace ShareHash{
    class SpinLock{
    public:
        SpinLock(){
            lock_.store(false);
        }
        ~SpinLock(){
            lock_.store(false);
        }

        void init(){
            lock_.store(false);
        }

        void lock(){
            while(lock_.exchange(true)){}
        }

        bool try_lock() {
            return !lock_.exchange(true);
        }

        void unlock(){
            while(!lock_.exchange(false)){}
        }

        bool test(){
            return lock_.load();
        }

        void wait(){
            while(lock_.load()){};
        }
    private:
        std::atomic_bool lock_;


    };

}

#endif //SHAREHASH_CONCURRENCY_H
