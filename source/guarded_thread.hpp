// RAII class for std::thread
#ifndef guarded_thread_hpp
#define guarded_thread_hpp

#include <stdio.h>
#include <thread>

struct guarded_thread:std::thread {
    using thread::thread;
    guarded_thread(const guarded_thread&) = delete;
    guarded_thread(guarded_thread&&) = default;
    ~guarded_thread(){if(this->joinable()) this->join();}
};

#endif /* guarded_thread_hpp */