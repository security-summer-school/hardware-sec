#include <stdio.h>
#include <immintrin.h> /* the compiler intrinsic functions */

void get_rdrand(void)
{
	unsigned long long rand_long;
	unsigned int rand_int;
	unsigned short rand_short;
	int rc;

	rc = _rdrand64_step(&rand_long);
	if (rc != 1) {
		printf("Error: rdrand failed\n");
		rand_long = 0;
	}
	rc = _rdrand32_step(&rand_int);
	if (rc != 1) {
		printf("Error: rdrand failed\n");
		rand_int = 0;
	}
	rc = _rdrand16_step(&rand_short);
	if (rc != 1) {
		printf("Error: rdrand failed\n");
		rand_short = 0;
	}

	printf("[RDRAND] The random long int is: %llu\n", rand_long);
	printf("[RDRAND] The random int is: %u\n", rand_int);
	printf("[RDRAND] The random short int is: %hu\n", rand_short);
}
