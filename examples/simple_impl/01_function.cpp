// std::function 简单实现
//
// 视频 https://www.youtube.com/watch?v=xJSKk_q25oQ

template <typename T>
class function;

template <typename Ret, typename ... Param>
class function<Ret (Param...)> {
};

int main() {
    
    // 编译错误
    // function<int> f1;

    function<int (int)> f;

    return 0;
}