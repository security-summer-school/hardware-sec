#include <stdio.h>
#include <immintrin.h> /* the compiler intrinsic functions */

void get_rdseed(void)
{
	unsigned long long rand_long;
	unsigned int rand_int;
	unsigned short rand_short;
	int rc;

	rc = _rdseed64_step(&rand_long);
	if (rc != 1) {
		printf("Error: rdseed failed\n");
		rand_long = 0;
	}
	rc = _rdseed32_step(&rand_int);
	if (rc != 1) {
		printf("Error: rdseed failed\n");
		rand_int = 0;
	}
	rc = _rdseed16_step(&rand_short);
	if (rc != 1) {
		printf("Error: rdseed failed\n");
		rand_short = 0;
	}

	printf("[RDSEED] The random long int is: %llu\n", rand_long);
	printf("[RDSEED] The random int is: %u\n", rand_int);
	printf("[RDSEED] The random short int is: %hu\n", rand_short);
}
