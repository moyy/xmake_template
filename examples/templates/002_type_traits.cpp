#include <type_traits>

namespace detail {
    template<typename T>
    constexpr bool is_class_or_union(int T::*) {
        return not std::is_union<T>::value;
    }

    template<typename T>
    constexpr bool is_class_or_union(...) {
        return false;
    }
}

template <typename T>
using is_class = std::bool_constant<detail::is_class_or_union<T>(nullptr)>;

int main() {
    struct Foo {};
    static_assert(is_class<Foo>::value);
    return 0;
}