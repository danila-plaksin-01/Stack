#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#define POISON              -666           // яд
#define STEP                2                // шаг realloc
#define CAP_MIN             2             // минимальный размер стека
#define FAIL                4189             // кoд ошибки
#define ERR_NULLPTR         7799      // нулевой указатель массива стека
#define ERR_BIG_COUNT       685     // большой счетчик
#define ERR_LITTLE_COUNT    9855 // маленький счетчик
#define ERR_REALLOC         4319      // realloc не увеличил массив
#define ERR_BAD_POINTER     9875  // нулевой указатель на стек
#define ERR_CANARY_1        4171     // испорчена первая канарейка
#define ERR_CANARY_2        4172     // испорчена вторая канарейка
#define ERR_CANARY_BUF_1    4641 // испорчена 1 канарейка в буфере
#define ERR_CANARY_BUF_2    4642 // испорчена вторая канарейка в буфере
#define ERR_BUF             3674          // не совпадает указатель на buffer_
#define ERR_DATA            3151         // не совпадает указатель на data_
#define ERR_PSEUDO_COPY     4906  // не совпадает указатель на копию
#define ERR_NUMB_FUNC       7477 // не совпадает номер хэш-функции
#define ERR_HASH            6126 // не совпадает хэш
#define NEW                 732
#define CONSTRUCT           4972
#define PUSH_FIRST          7261
#define PUSH_SECOND         7262
#define POP_FIRST           91
#define POP_SECOND          92
#define DESTRUCT            4325
#define NUMB_HASH_FUNC      3

typedef unsigned long long int CanaryT;

template <typename SizeT, typename ElemT>
struct Stack_Copy{
    CanaryT canary_1_;
    ElemT* data_;
    void* buffer_;
    CanaryT canary_2_;
    CanaryT can_buf_1;
    CanaryT can_buf_2;
    unsigned int Hash_;
    unsigned int Numb_of_hash_func_;
};

template <typename SizeT, typename ElemT>
class Stack
{
private:
    CanaryT canary_1_;
    SizeT cap_;
    SizeT size_;
    SizeT size_buf_; // размер буфера в байтах
    ElemT *data_;
    CanaryT Canary_Init_();
    void* buffer_;
    void New();
    int errcode_;
    char* Error_Processing_();/* Функция обработки ошибок*/
    Stack_Copy<SizeT, ElemT>* Pseudo_Copy_;/* Указатель на структуру,
    хранящуюю предыдущие значения переменных*/
    unsigned int Hash_;
    unsigned int Numb_of_hash_func_;
public:
    void Dump();
    int Push(ElemT);
    int Pop(ElemT *);
    int Construct();
    int Destruct();
    int OK(unsigned int);

private:
    CanaryT canary_2_;
};

unsigned int HashFAQ6(const char * str, unsigned int size);
unsigned int HashRot13(const char * str, unsigned int size);
unsigned int HashLy(const char * str, unsigned int size);
