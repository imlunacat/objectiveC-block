#include <stdio.h>
int main() {
    void (^b)(void) = ^{ printf("Hello Block"); };
    b();
	return 0;
}