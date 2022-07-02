#include <stdio.h>
#include <immintrin.h> // the compiler intrinsic functions

void get_rdrand(void)
{
	unsigned long long rand_long;
	unsigned int rand_int;
	unsigned short rand_short;

	/* TODO: Fill the variables with random numbers, using _rdrand<size>_step() */

	printf("[RDRAND] The random long int is: %llu\n", rand_long);
	printf("[RDRAND] The random int is: %u\n", rand_int);
	printf("[RDRAND] The random short int is: %hu\n", rand_short);
}
