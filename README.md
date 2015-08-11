# Objective-C Block
## Introduction


## Difference between function pointer & block?
* block is the ability to use variables from the surrounding scope <br>
Block 可以用外部變數. Function pointer 不可以 <br>

```objectivec
__block int temp = 5;
typedef int (^myblock_type)(int a);
myblock_type myblock = ^int(int a){
    temp++;
    return temp;
};
```
## Why Block
### UIAnimation
**Without block**

```objectivec
[UIView beginAnimations:@"Example" context:nil];
[UIView setAnimationDuration:5.0];
[UIView setAnimationDidStopSelector:@selector(removeAnimationView)];
[animatingView setAlpha:0];
[animatingView setCenter:CGPointMake(animatingView.center.x+50.0, 
                                     animatingView.center.y+50.0)];
[UIView commitAnimations];
```
**With Block**

```objectivec
[UIView animateWithDuration:5.0 
                 animations:^{
                    [animatingView setAlpha:0];
                    [animatingView setCenter:CGPointMake(animatingView.center.x+50.0, 
                                                         animatingView.center.y+50.0)];
                 } 
                 completion:^(BOOL finished) {
                     [animatingView removeFromSuperview];
                 }];
```
<cite>[Introduction of Block - Ray Wenderlich]</cite>

### Delegation
**Without Block**

```objectivec
- (IBAction)grabURLInBackground:(id)sender
{
   NSURL *url = [NSURL URLWithString:@"http://allseeing-i.com"];
   ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
   [request setDelegate:self];
   [request startAsynchronous];
}
 
// lot of not related code
- (void)requestFinished:(ASIHTTPRequest *)request
{
   // Use when fetching text data
   NSString *responseString = [request responseString];
 
   // Use when fetching binary data
   NSData *responseData = [request responseData];
}
 
- (void)requestFailed:(ASIHTTPRequest *)request
{
   NSError *error = [request error];
}
```
**With Block**

```objective-c
- (IBAction)grabURLInBackground:(id)sender
{
   NSURL *url = [NSURL URLWithString:@"http://allseeing-i.com"];
   __block ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
   [request setCompletionBlock:^{
      // Use when fetching text data
      NSString *responseString = [request responseString];
 
      // Use when fetching binary data
      NSData *responseData = [request responseData];
   }];
   [request setFailedBlock:^{
      NSError *error = [request error];
   }];
   [request startAsynchronous];
}
```
<cite>[ASIHTTPRequest document]</cite>

* Keep your code together ( Simplify code )
* Apple said so [UIView Class Reference - Animation]

## Block Implementation ( In High-Level View )
A block consists:

```objectivec
struct Block_literal_1 {
    void *isa; // initialized to &_NSConcreteStackBlock or &_NSConcreteGlobalBlock
    int flags;
    int reserved;
    void (*invoke)(void *, ...);
    struct Block_descriptor_1 {
    unsigned long int reserved;         // NULL
        unsigned long int size;         // sizeof(struct Block_literal_1)
        // optional helper functions
        void (*copy_helper)(void *dst, void *src);     // IFF (1<<25)
        void (*dispose_helper)(void *src);             // IFF (1<<25)
        // required ABI.2010.3.16
        const char *signature;                         // IFF (1<<30)
    } *descriptor;
    // parameters if any
};
```
<br>
**Single block**

```objectivec
void (^b)(void) = ^{ printf("Hello Block"); };
b();
```

will translated to

```objectivec
struct __block_impl {
    void *isa;
    int Flags;
    int Reserved;
    void *FuncPtr;
};

struct __main_block_impl_0 {
    struct __block_impl impl;
    struct __main_block_desc_0* Desc;
    // Constructor of __main_block_impl_0
    __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, int flags=0) {
        impl.isa = &_NSConcreteStackBlock;
        impl.Flags = flags;
        impl.FuncPtr = fp;
        Desc = desc;
    }
};
static void __main_block_func_0(struct __main_block_impl_0 *__cself) {
    printf("Hello Block"); 
}

static struct __main_block_desc_0 {
    size_t reserved;
    size_t Block_size;
} __main_block_desc_0_DATA = { 
    0, 
    sizeof(struct __main_block_impl_0)
};
```
and Block

```objectivec
// void (^b)(void) = ^{ printf("Hello Block"); };
// void (*b)(void) = (void (*)())&__main_block_impl_0((void *)__main_block_func_0, &__main_block_desc_0_DATA);
struct __block_impl impl = {&_NSConcreteStackBlock, 0, 0, (void *)__main_block_func_0};
struct __main_block_impl_0 tmp = {impl, &__main_block_desc_0_DATA};
struct __main_block_impl_0 *b = &tmp;

//((void (*)(__block_impl *))((__block_impl *)b)->FuncPtr)((__block_impl *)b);
__block_impl *func = ((__block_impl *)b);
(((void (*)(__main_block_impl_0 *))func->FuncPtr)(b));
```
Compilable file: [print.cc]
<br><br>
**Block with paramter**

```objecticec
int x = 10;
void (^vv)(void) = ^{ printf("x is %d\n", x); }
x = 11;
vv();
```
will translated to

```objectivec
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
  __main_block_impl_0(void *fp, struct __main_block_desc_0 *desc, int _x, int flags=0) : x(_x) {
      impl.isa = &_NSConcreteStackBlock;
      impl.Flags = flags;
      impl.FuncPtr = fp;
      Desc = desc;
  }
};

static void __main_block_func_0(struct __main_block_impl_0 *__cself) {
    int x = __cself->x; // bound by copy
    printf("x is %d\n", x); 
}

static struct __main_block_desc_0 {
    size_t reserved;
    size_t Block_size;
} __main_block_desc_0_DATA = { 0, sizeof(struct __main_block_impl_0)};
```

and main ( with Block )

```objectivec
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
```
Compilable file: [print_var.cc]
<br><br>
**Block with Block**

```objectivec
void (^existingBlock)(void) = ...;
void (^vv)(void) = ^{ existingBlock(); }
vv();
```

will translated to

```objectivec
struct __block_literal_3 {
   ...; // existing block
};

struct __block_literal_4 {
    void *isa;
    int flags;
    int reserved;
    void (*invoke)(struct __block_literal_4 *);
    struct __block_descriptor_4 *descriptor; // not list in doc, mayber a doc ERROR ?
    struct __block_literal_3 *const existingBlock;
};

void __block_invoke_4(struct __block_literal_4 *_block) {
   __block->existingBlock->invoke(__block->existingBlock);
}

void __block_copy_4(struct __block_literal_4 *dst, struct __block_literal_4 *src) {
     // copying a Block declared variable from the stack Block to the heap
     // see http://opensource.apple.com/source/clang/clang-137/src/projects/compiler-rt/BlocksRuntime/runtime.c
     
     _Block_object_assign(&dst->existingBlock, src->existingBlock, BLOCK_FIELD_IS_BLOCK);
}

void __block_dispose_4(struct __block_literal_4 *src) {
      // get rid of a referenced object held by this Block
      // see http://opensource.apple.com/source/clang/clang-137/src/projects/compiler-rt/BlocksRuntime/runtime.c

     _Block_object_dispose(src->existingBlock, BLOCK_FIELD_IS_BLOCK);
}

static struct __block_descriptor_4 {
    unsigned long int reserved;
    unsigned long int Block_size;
    // Note. helper is no need unless we use no-const
    void (*copy_helper)(struct __block_literal_4 *dst, struct __block_literal_4 *src);
    void (*dispose_helper)(struct __block_literal_4 *);
} __block_descriptor_4 = {
    0,
    sizeof(struct __block_literal_4),
    __block_copy_4,
    __block_dispose_4,
};
```

```objectivec
struct __block_literal_4 _block_literal = {
      &_NSConcreteStackBlock,  // isa
      (1<<25)|(1<<29), 			   // flags
      <uninitialized>	        // reserved
      __block_invoke_4,        // invoke
      & __block_descriptor_4   // descriptor4
      existingBlock,           
};
```
<br><br>
**Block use _block variables**

Basic block layout

```objectivec
struct _block_byref_foo {
    void *isa; // NULL
    struct _block_byref_foo *forwarding; // beginning of its enclosing structure
    int flags;   //refcount;
    int size; // size of the enclosing structure
    // Optional
    // helper functions called via Block_copy() and Block_release()
    void (*byref_keep)(void  *dst, void *src);
    void (*byref_dispose)(void *);
    typeof(marked_variable) marked_variable;
};
```
**_block variable**

```objectivec
int __block i = 2;
i = 3;
```
will translated to

```objectivec
struct __Block_byref_i_0 {
    void *__isa;
    __Block_byref_i_0 *__forwarding;
    int __flags;
    int __size;
    int i;
};
 // main
__Block_byref_i_0 i = {
  (void*)0,                  // isa
  (__Block_byref_i_0 *)&i,   // forwarding
  0,                         // flag
  sizeof(__Block_byref_i_0), // size
  2                          // i
};

(i.__forwarding->i) = 3;
```
**uses __block**

```objectivec
int __block i = 2;
functioncall(^{ i = 10; });
```

will translated to

```objectivec
/* int __block i = 2; */
struct _block_byref_i {
    void *isa;  // set to NULL
    struct _block_byref_voidBlock *forwarding;
    int flags;   //refcount;
    int size;
    void (*byref_keep)(struct _block_byref_i *dst, struct _block_byref_i *src);
    void (*byref_dispose)(struct _block_byref_i *);
    int captured_i;
};
struct _block_byref_i i = {( 
.forwarding=&i, 
.flags=0, 
.size=sizeof(struct _block_byref_i) )}; // isa = NULL
```
## Notes
block in another language: Java, C, Java8<br>
Execution in the Kingdom of Nouns<br>
http://www.opensource.apple.com/source/libclosure/libclosure-53/BlockImplementation.txt<br>

* Block are first-class citizen
[block introduction in raywenderlich] (http://www.raywenderlich.com/9438/how-to-use-blocks-in-ios-5-tutorial-part-2) <br>

> * Blocks are regular Object in Objective-C <br>
> * Block can be used as parameter, return values, etc [First-class function-wikipedia]
## References
[Introduction of Block - Ray Wenderlich]:http://www.raywenderlich.com/9438/how-to-use-blocks-in-ios-5-tutorial-part-2

[ASIHTTPRequest document]:http://allseeing-i.com/ASIHTTPRequest/How-to-use

[UIView Class Reference - Animation]:https://developer.apple.com/library/ios/documentation/UIKit/Reference/UIView_Class/index.html#//apple_ref/doc/uid/TP40006816-CH3-SW131

[First-class function-wikipedia]:https://en.wikipedia.org/wiki/First-class_function

[Working with Blocks - iOS Developer Library]:https://developer.apple.com/library/ios/documentation/Cocoa/Conceptual/ProgrammingWithObjectiveC/WorkingwithBlocks/WorkingwithBlocks.html

[Block Spec - Clang]:http://clang.llvm.org/docs/Block-ABI-Apple.html

[clang runtime.c]:http://opensource.apple.com/source/clang/clang-137/src/projects/compiler-rt/BlocksRuntime/runtime.c

[print.cc]:https://github.com/imlunacat/objectiveC-block/blob/master/print.cc

[print_var.cc]:https://github.com/imlunacat/objectiveC-block/blob/master/print_var.cc