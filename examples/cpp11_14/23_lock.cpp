// + std::lock_gurad 更轻量，少一些函数；
// + std::unique_lock 有 unlock 函数，手动释放锁
// 因为 条件变量 在 wait时 需要 手动释放锁，所以 条件变量 + std::unique_lock 使用。

#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>

int main() {

    std::mutex mutex_;
    
    auto func = [&mutex_](int k) {
        // std::lock_guard<std::mutex> lock(mutex_);
        std::unique_lock<std::mutex> lock(mutex_);
        
        for (int i = 0; i < k; ++i) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    };

    std::thread threads[5];
    for (int i = 0; i < 5; ++i) {
        threads[i] = std::thread(func, 200);
    }
 
    for (auto& th : threads) {
        th.join();
    }
    return 0;
}