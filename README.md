#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
	
	srand(time(NULL));
	int a;
	a = rand() % 23 + 1;
	printf("%d", a);
}
