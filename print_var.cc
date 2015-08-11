#include <stdio.h>
struct __block_impl {
    void *isa;
    int Flags;
    int Reserved;
    void *FuncPtr;
};

struct __main_block_impl_0 {
    struct __block_impl impl;
    struct __main_block_desc_0* Desc;
    int x;
    // __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, int _x, int flags=0) : x(_x) {
    // 	   impl.isa = _NSConcreteStackBlock;
    //     impl.Flags = flags;
    //     impl.FuncPtr = fp;
    //     Desc = desc;
    // }
};
static void __main_block_func_0(struct __main_block_impl_0 *__cself) {
	int x = __cself->x; // bound by copy
    printf("x is %d\n", x); 
}

static struct __main_block_desc_0 {
	size_t reserved;
  	size_t Block_size;
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0)};

int main() {
 	int x = 10;

    // void (^vv)(void) = ^{ printf("x is %d\n", x); };
 	// void (*vv)(void) = (void (*)())&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA, x);
    struct __block_impl impl = {NULL, 0, 0, (void *)__main_block_func_0};
    struct __main_block_impl_0 tmp = {impl, &__main_block_desc_0_DATA, x};
    struct __main_block_impl_0 *vv = &tmp;

 	x = 11;
 	// vv();
 	// ((void (*)(__block_impl *))((__block_impl *)vv)->FuncPtr)((__block_impl *)vv);
 	__block_impl *func = ((__block_impl *)vv);
 	(((void (*)(__main_block_impl_0 *))func->FuncPtr)(vv));
	return 0;
}