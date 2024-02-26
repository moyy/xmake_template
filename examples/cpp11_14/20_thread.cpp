#include <iostream>
#include <thread>

int main() {
    auto func = [](int k) {
        for (int i = 0; i < k; ++i) {
            std::cout << std::this_thread::get_id() << ": " << i << std::endl;
        }
    };

    std::thread t1(func, 50);
    std::thread t2(func, 60);

    // 16 个 超线程
    std::cout << "cpu core num = " << std::thread::hardware_concurrency() << std::endl;

    // 休眠 1s
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 注：join 堵塞 调用线程 直到 t 结束；
    // 注：detach 是 将 线程 与 thread 对象 管理分离
    //    - 意思是 一旦调用了detach，则 std::thread 调用析构之后，线程有可能还在运行；
    if (t1.joinable()) { // 检查线程可否被join
        t1.join();   // 堵塞
        // t1.detach(); // 分离线程
    }
 
    if (t2.joinable()) { // 检查线程可否被join
        t2.join();   // 堵塞
        // t2.detach(); // 分离线程
    }

    return 0;
}