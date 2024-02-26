#include <iostream>

class MyString {
public:
    MyString() {
        std::cout << "MyString::MyString()" << std::endl;

        this->str = new char[1];
        *str = '\0';
    }

    MyString(const char *str = "") {
        if (!str) {
            str = "";
        }

        std::cout << "MyString::MyString(const char *str)" << std::endl;

        this->str = new char[strlen(str) + 1];
        strcpy(this->str, str);
        this->str[strlen(str)] = '\0';
    }

    MyString(const MyString &other) {
        std::cout << "MyString::MyString(const MyString &other)" << std::endl;

        this->str = new char[strlen(other.str) + 1];
        strcpy(this->str, other.str);
        this->str[strlen(other.str)] = '\0';
    }

    MyString &operator=(const MyString &other) =delete;
    
    MyString(MyString &&other) =delete;

    MyString &operator=(MyString &&other) =delete;

    ~MyString() {
        delete[] this->str;
    }
private:
    char *str;
};

int main() {

    std::cout << "============== 1: " << std::endl;
    MyString dots("abc");       // 直接初始化, MyString::MyString(const char *str)
    
    std::cout << "============== 2: " << std::endl;
    MyString s(dots);           // 拷贝初始化，MyString::MyString(const MyString &other)
    
    std::cout << "============== 3: " << std::endl;
    MyString s2 = dots;         // 拷贝初始化，MyString::MyString(const MyString &other)
    
    std::cout << "============== 4: " << std::endl;
    MyString myBook = "9-999";  // 直接初始化，MyString::MyString(const char *str)
    
    std::cout << "============== 5: " << std::endl;
    MyString lines = MyString("abcdefg"); // 直接初始化，MyString::MyString(const char *str)

    return 0;
}