
// 条件变量 阻塞 线程 直到 有线程通知 或 超时 才会 唤醒正在阻塞的线程
// 条件变量 需要 配合 std::unique_lock 使用

#include <condition_variable>
#include <mutex>

class CountDownLatch
{
public:
    explicit CountDownLatch(uint32_t count) : count_(count) {

    }

    void count_down()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        --count_;
        if (count_ == 0)
        {
            cv_.notify_all();
        }
    }

    void await(uint32_t time_ms = 0)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (count_ > 0)
        {
            if (time_ms > 0)
            {
                cv_.wait_for(lock, std::chrono::milliseconds(time_ms));
            }
            else
            {
                cv_.wait(lock);
            }
        }
    }

    uint32_t get_count() const
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return count_;
    }

private:
    std::condition_variable cv_;
    mutable std::mutex mutex_;
    uint32_t count_ = 0;
};

int main() {
    return 0;
}