#include <stdio.h>
#include <stdlib.h>

struct A{
	int a;
	double b;
	char c;
	float d;
};

int main(){
	struct A a;
	printf("%lu\n", sizeof(a));
	return 0;

}
