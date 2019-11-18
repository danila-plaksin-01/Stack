#include "Stack_v4.cpp"
#include <stdio.h>
#include <stdlib.h>

int main()
    {
    Stack<unsigned int, int> s = {};
    s.Construct();
    s.Push(20);
    s.Push(30);
    s.Push(2);
    s.Push(6);
    int a = 0;
    int b = 0;
    s.Pop(&a);
    s.Pop(&b);
    printf("%d, %d\n", a, b);
    s.Dump();
    s.Destruct();
    return 0;
    }