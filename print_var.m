#include <stdio.h>
int main() {
	int x = 10;
	void (^vv)(void) = ^{ printf("x is %d\n", x); };
	x = 11;
	vv();

	return 0;
}