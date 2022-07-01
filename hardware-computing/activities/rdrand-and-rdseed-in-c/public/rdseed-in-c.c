#include <stdio.h>
#include <immintrin.h> // the compiler intrinsic functions

void get_rdseed(void)
{
	unsigned long long rand_long;
	unsigned int rand_int;
	unsigned short rand_short;

	/* TODO: Fill the variables with random numbers, using _rdseed<size>_step()
	 */

	printf("[RDSEED] The random long int is: %llu\n", rand_long);
	printf("[RDSEED] The random int is: %u\n", rand_int);
	printf("[RDSEED] The random short int is: %hu\n", rand_short);
}
