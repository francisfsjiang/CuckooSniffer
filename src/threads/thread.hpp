#ifndef CUCKOOSNIFFER_THREADS_THREAD_HPP
#define CUCKOOSNIFFER_THREADS_THREAD_HPP

#include <thread>
#include <vector>

namespace cs::threads{

    void start_threads(int);

    extern int THREADS_NUM;

    extern std::vector<std::thread> THREADS_POOL;

}


#endif //CUCKOOSNIFFER_THREAD_HPP
