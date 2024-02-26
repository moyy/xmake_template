// std::mutex 互斥量，分为 4 种：
// 
// + std::mutex 不可重入，不带超时功能
// + std::recursive_mutex 可重入
// + std::timed_mutex 带超时 的 互斥量，不可重入
// + std::recursive_timed_mutex 带超时的互斥量，可重入

#include <iostream>
#include <mutex>
#include <thread>

int main() {
    
    std::mutex mutex_;

    auto func1 = [&mutex_](int k) {
        mutex_.lock();
    
        std::cout << "Thread-" << std::this_thread::get_id() << ": ";

        for (int i = 0; i < k; ++i) {
            std::cout << i << " ";
        }
    
        std::cout << std::endl;
    
        mutex_.unlock();
    };

    std::thread threads[5];
    
    for (int i = 0; i < 5; ++i) {
        threads[i] = std::thread(func1, 20);
    }
    
    for (auto& th : threads) {
        th.join();
    }
 
    return 0;
}