// std::call_once 保证 某一函数 在 多线程环境中 只调用一次，需要配合 std::once_flag 使用

#include <mutex>
#include <thread>
#include <iostream>

std::once_flag onceflag;

void foo() {
    std::call_once(onceflag, []() {
        // 5个线程，只调用一次
        std::cout << "call once" << std::endl;
    });
}

int main() {
    std::thread threads[5];
 
    for (int i = 0; i < 5; ++i) {
        threads[i] = std::thread(foo);
    }
 
    for (auto& th : threads) {
        th.join();
    }
 
    return 0;
}