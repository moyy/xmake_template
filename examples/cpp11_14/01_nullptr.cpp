// 用 NULL 的话，无法区分 int 还是 void*
//     void f(int a);   // f(NULL)
//     void f(char *a); // f((char *)NULL) | f(nullptr)

#include <cassert>
#include <iostream>
#include <type_traits>

void foo(char *) {
    std::cout << "foo(char*) is called" << std::endl;
}
void foo(int i) {
    std::cout << "foo(int) is called" << std::endl;
}

int main() {

    static_assert(std::is_same<decltype(NULL), decltype(0)>::value);
    
    static_assert(! std::is_same<decltype(NULL), decltype((void*)0)>::value);
    
    static_assert(! std::is_same<decltype(NULL), std::nullptr_t>::value);

    static_assert(! std::is_same<decltype((void*)0), std::nullptr_t>::value);

    foo(0);          // 调用 foo(int)
    foo(nullptr);    // 调用 foo(char*)

    // foo(NULL);    // 该行不能通过编译

    return 0;
}

