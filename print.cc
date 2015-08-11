#include <stdio.h>

/*
clang -rewrite-objc
int main() {
    void (^b)(void) = ^{ printf("Hello Block"); };
    b();
    return 0;
}

This file is simplified and can be compiled (and executed).
*/
struct __block_impl {
    void *isa;
    int Flags;
    int Reserved;
    // void (*FuncPtr)(struct __main_block_impl_0 *);
    void *FuncPtr;
};

struct __main_block_impl_0 {
    // void *isa;
    // int Flags;
    // int Reserved;
    // void *FuncPtr;
    // size_t reserved;
    // size_t Block_size;

    struct __block_impl impl;
    struct __main_block_desc_0* Desc;
    // Constructor of __main_block_impl_0
    // __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, int flags=0) {
    //     impl.isa = NULL;
    //     impl.Flags = flags;
    //     impl.FuncPtr = fp;
    //     Desc = desc;
    // }
};

static struct __main_block_desc_0 {
    size_t reserved;
    size_t Block_size;
} __main_block_desc_0_DATA = { 
    20, 
    sizeof(struct __main_block_impl_0)
};

static void __main_block_func_0(struct __main_block_impl_0 *__cself) {
    printf("Hello Block %d %d\n", __cself->impl.Flags, __cself->Desc->reserved); 
    printf("Hello Block\n"); 
}


int main() {
    // void (^b)(void) = ^{ printf("Hello Block"); };
    // void (*b)(void) = (void (*)())&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA);
    struct __block_impl impl = {NULL, 10, 0, (void *)__main_block_func_0};
    struct __main_block_impl_0 tmp = {impl, &__main_block_desc_0_DATA};
    struct __main_block_impl_0 *b = &tmp;


    // b();
    // ((void (*)(__block_impl *))((__block_impl *)b)->FuncPtr)((__block_impl *)b);
    __block_impl *func = ((__block_impl *)b);
    (((void (*)(__main_block_impl_0 *))func->FuncPtr)(b));
    return 0;
}