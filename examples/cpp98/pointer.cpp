// 指针

int main() {

    int a;        // 普通 变量
    int *pa = &a; // 指向 int 的 指针
    
    int arr[3];   // 与 [] 结合，数组；元素类型是 int
    int *parr[3]; // 与 [] 结合，数组；元素类型是 int*

    int (*p)[3] = &arr; // p是指针，指向 int[3]

    int foo(int); // 函数

    int (*pf)(int) = foo; // pf 是 指针， 指向 int (int) 就是 函数

    int **pp = &pa; // 指向 指针的指针

    return 0;
}