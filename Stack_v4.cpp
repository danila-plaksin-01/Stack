#include "Stack.h"

typedef unsigned int (*Hash_FuncT)(const char* str, unsigned size);
Hash_FuncT Hash_Func[] = {HashFAQ6, HashRot13, HashLy};

template <typename SizeT, typename ElemT>
int Stack<SizeT, ElemT>::OK(unsigned int from)
{
    if (data_ == NULL)
        return ERR_NULLPTR;

    if (buffer_ == NULL)
        return ERR_NULLPTR;

    if (Pseudo_Copy_ == NULL)
        return ERR_NULLPTR;

    if (size_ > cap_)
        return ERR_BIG_COUNT;

    if (size_ < 0)
        return ERR_LITTLE_COUNT;
    
    if ((from == POP_FIRST) && (size_ <= 0))
        return ERR_LITTLE_COUNT;

    static Stack_Copy<SizeT, ElemT>* Pref_P_C = Pseudo_Copy_;
    if (Pseudo_Copy_ != Pref_P_C)
        return ERR_PSEUDO_COPY;

    if (Pseudo_Copy_->Numb_of_hash_func_ != Numb_of_hash_func_)
        return ERR_NUMB_FUNC;

    Hash_ = Hash_Func[Numb_of_hash_func_]((const char*)buffer_, size_buf_);
    if ((from != POP_SECOND) && (from != PUSH_SECOND) && (from != NEW) && (Pseudo_Copy_->Hash_ != Hash_))
        return ERR_HASH;

    if (Pseudo_Copy_->can_buf_1 != *(CanaryT*)buffer_)
        return ERR_CANARY_BUF_1;

    if (Pseudo_Copy_->can_buf_2 != *(CanaryT*)(&data_[cap_]))
        return ERR_CANARY_BUF_2;

    if (Pseudo_Copy_->canary_1_ != canary_1_)
        return ERR_CANARY_1;

    if (Pseudo_Copy_->canary_2_ != canary_2_)
        return ERR_CANARY_2;
    
    if ((from != NEW) && (Pseudo_Copy_->buffer_ != buffer_))
        return ERR_BUF;
    
    if ((from != NEW) && (Pseudo_Copy_->data_ != data_))
        return ERR_DATA;

    Pseudo_Copy_->can_buf_1 = *(CanaryT*)buffer_;
    Pseudo_Copy_->can_buf_2 = *(CanaryT*)(&data_[cap_]);
    Pseudo_Copy_->canary_1_ = canary_1_;
    Pseudo_Copy_->canary_2_ = canary_2_;
    Pseudo_Copy_->buffer_ = buffer_;
    Pseudo_Copy_->data_ = data_;
    Pseudo_Copy_->Hash_ = Hash_;

    return 0;
}
#define ASSERT_OK(Name_of_Func)   if ((errcode_ = OK(Name_of_Func)) != 0)\
                    {\
                        Dump();\
                        assert(!#Name_of_Func);\
                    }

template <typename SizeT, typename ElemT>
CanaryT Stack<SizeT, ElemT>::Canary_Init_()
{
    static bool first = true;//Устанавливаем семя рандома
    if (first)
    {
        first = false;
        srand(time(0));
    }
    
    unsigned int Up = sizeof(CanaryT) * 2;
    CanaryT canary = 0;
    for (int i = 0; i < Up; ++i)
        canary = rand() % 16 + canary * 16;
    return canary;
}

template <typename SizeT, typename ElemT>
void Stack<SizeT, ElemT>::Dump()
{
    FILE* fl = fopen("Stack_Dump.txt", "w");
    fprintf(fl, "Stack_Dump:\t\t");
    if (errcode_)
        fprintf(fl, "ERROR!!!");
    else
        fprintf(fl, "ok");
    fprintf(fl, "\n\n");
    fprintf(fl, "\tStack [%p]\n", this);
    fprintf(fl, "\t{\n");
    fprintf(fl, "\t\terrcode_ = %d %s\n", errcode_, Error_Processing_());
    fprintf(fl, "\t\tcanary_1_ = 0x%llX\n", canary_1_);
    fprintf(fl, "\t\tcap_ = %d\n", cap_);
    fprintf(fl, "\t\tsize_ = %d\n", size_);
    fprintf(fl, "\t\tsize_buf_ = %d\n", size_buf_);
    fprintf(fl, "\t\tbuffer_ = [%p]\n", buffer_);
    fprintf(fl, "\t\t\tcanary_buf_1 = 0x%llX\n", *(CanaryT*)buffer_);
    fprintf(fl, "\t\tdata_ = [%p]\n", data_);
    fprintf(fl, "\t\t{\n");
    for (int i = 0; i < cap_; ++i)
    {
        if (i < size_)
            fprintf(fl, "\t\t\t*[%d] = %d", i, data_[i]);
        else
            fprintf(fl, "\t\t\t [%d] = %d", i, data_[i]);
        if (data_[i] == (ElemT)POISON)
            fprintf(fl, "\t(Poison???)");
        fprintf(fl, "\n");
    }
    fprintf(fl, "\t\t}\n");
    fprintf(fl, "\t\tcanary_buf_2 = 0x%llX\n", *(CanaryT*)(&data_[cap_]));
    fprintf(fl, "\t\tHash_ = %lld\n", Hash_);
    fprintf(fl, "\t\tNumb_of_hash_func = %d\n", Numb_of_hash_func_);
    fprintf(fl, "\t\tcanary_2_ = 0x%llX\n", canary_2_);
    fprintf(fl, "\t}\n");
    fclose(fl);
}

#define CASE(ERR)   case ERR:\
                        return (char*)"("#ERR")";

template <typename SizeT, typename ElemT>
char* Stack<SizeT, ElemT>::Error_Processing_()
{
    switch (errcode_)
    {
    case 0:
        return (char*) "(No errors)";
    CASE(ERR_NULLPTR)     
    CASE(ERR_BIG_COUNT)
    CASE(ERR_LITTLE_COUNT)
    CASE(ERR_REALLOC)  
    CASE(ERR_BAD_POINTER) 
    CASE(ERR_CANARY_1)    
    CASE(ERR_CANARY_2)   
    CASE(ERR_CANARY_BUF_1)
    CASE(ERR_CANARY_BUF_2)
    CASE(ERR_BUF)         
    CASE(ERR_DATA)        
    CASE(ERR_PSEUDO_COPY)   
    CASE(ERR_NUMB_FUNC)
    CASE(ERR_HASH)     
    default:
        return (char*)"(Unknown error)";  
    }
}

template <typename SizeT, typename ElemT>
int Stack<SizeT, ElemT>::Construct()
{
    if (this == NULL)
        assert(ERR_NULLPTR);
    cap_ = CAP_MIN;
    size_ = 0;
    size_buf_ = cap_ * sizeof(data_[0]) + sizeof(CanaryT) * 2;
    buffer_ = (void*) calloc(size_buf_, sizeof(char));
    data_ = (ElemT*) ((char*)buffer_ + sizeof(CanaryT));
    for (int i = size_; i < cap_; ++i)
        data_[i] = POISON;
    canary_1_ = Canary_Init_();
    canary_2_ = Canary_Init_();

    Numb_of_hash_func_ = rand() % NUMB_HASH_FUNC;// Выбираем хэш-функцию

    CanaryT canary_buf_1 = Canary_Init_();
    CanaryT canary_buf_2 = Canary_Init_();

    *(CanaryT*)buffer_ = canary_buf_1;
    *(CanaryT*)((char*)buffer_ + cap_ * sizeof(data_[0]) + sizeof(CanaryT)) = canary_buf_2;

    Hash_ = Hash_Func[Numb_of_hash_func_]((const char*)buffer_, size_buf_);
    
    /*Создаем и инициализируем копии*/
    Pseudo_Copy_ = (Stack_Copy<SizeT, ElemT>*)calloc(1, sizeof(Pseudo_Copy_[0]));
    Pseudo_Copy_->Numb_of_hash_func_ = Numb_of_hash_func_;
    Pseudo_Copy_->Hash_ = Hash_;
    Pseudo_Copy_->buffer_ = buffer_;
    Pseudo_Copy_->data_ = data_;
    Pseudo_Copy_->can_buf_1 = canary_buf_1;
    Pseudo_Copy_->can_buf_2 = canary_buf_2;
    Pseudo_Copy_->canary_1_ = canary_1_;
    Pseudo_Copy_->canary_2_ = canary_2_;

    ASSERT_OK(CONSTRUCT)
    return 0;
}

template <typename SizeT, typename ElemT>
int Stack<SizeT, ElemT>::Push(ElemT value)
{
    ASSERT_OK(PUSH_FIRST)
    if (cap_ == size_)
        New();
    data_[size_++] = value;
    ASSERT_OK(PUSH_SECOND)
    return 0;
}

template <typename SizeT, typename ElemT>
void Stack<SizeT, ElemT>::New()
{
    cap_ += STEP;
    size_buf_ = cap_ * sizeof(data_[0]) + sizeof(CanaryT) * 2;
    buffer_ = (void*) realloc(buffer_, size_buf_);
    data_ = (ElemT*)((char*)buffer_ + sizeof(CanaryT));
    *(CanaryT*)(&data_[cap_]) = *(CanaryT*)(&data_[cap_ - STEP]);
    for(int i = size_; i < cap_; ++i)
        data_[i] = POISON;
    ASSERT_OK(NEW);
}

template <typename SizeT, typename ElemT>
int Stack<SizeT, ElemT>::Pop(ElemT *a)
{
    ASSERT_OK(POP_FIRST)
    if (a == NULL)
        assert(!"Bad pointer to var (*a)");
    if (size_ <= 0)
        assert(!"POP: size_ <= 0");
    (*a) = data_[--size_];
    data_[size_] = POISON;
    ASSERT_OK(POP_SECOND)
    return 0;
}

template <typename SizeT, typename ElemT>
int Stack<SizeT, ElemT>::Destruct()
{
    ASSERT_OK(DESTRUCT)
    cap_ = 0;
    size_ = 0;
    free(buffer_);
    free(Pseudo_Copy_);
    return 0;
}

unsigned int HashFAQ6(const char * str, unsigned int size)
{
    unsigned int hash = 0;

    for (int i = 0; i < size; i++)
    {
        hash += (unsigned char)str[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

unsigned int HashRot13(const char * str, unsigned int size)
{
    unsigned int hash = 0;

    for (int i = 0; i < size; i++)
    {
        hash += (unsigned char)str[i];
        hash -= (hash << 13) | (hash >> 19);
    }

    return hash;
}

unsigned int HashLy(const char * str, unsigned int size)
{
    unsigned int hash = 0;

    for (int i = 0; i < size; i++)
        hash = (hash * 1664525) + (unsigned char)str[i] + 1013904223;

    return hash;
}