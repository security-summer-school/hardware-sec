#include <stdio.h>

extern unsigned long long get_rdrand64(void);
extern unsigned int get_rdrand32(void);
extern unsigned short get_rdrand16(void);

extern unsigned long long get_rdseed64(void);
extern unsigned int get_rdseed32(void);
extern unsigned short get_rdseed16(void);

int main(void)
{
	unsigned long long rand_long;
	unsigned int rand_int;
	unsigned short rand_short;

	rand_long = get_rdrand64();
	if (rand_long)
		printf("[RDRAND] The random long int is: %llu\n", rand_long);
	else
		printf("Error: returned value not valid\n");

	rand_int = get_rdrand32();
	if (rand_int)
		printf("[RDRAND] The random int is: %u\n", rand_int);
	else
		printf("Error: returned value not valid\n");

	rand_short = get_rdrand16();
	if (rand_short)
		printf("[RDRAND] The random short int is: %hu\n", rand_short);
	else
		printf("Error: returned value not valid\n");

	rand_long = get_rdseed64();
	if (rand_long)
		printf("[RDSEED] The random long int is: %llu\n", rand_long);
	else
		printf("Error: returned value not valid\n");

	rand_int = get_rdseed32();
	if (rand_int)
		printf("[RDSEED] The random int is: %u\n", rand_int);

	rand_short = get_rdseed16();
	if (rand_short)
		printf("[RDSEED] The random short int is: %hu\n", rand_short);
	else
		printf("Error: returned value not valid\n");

	return 0;
}
