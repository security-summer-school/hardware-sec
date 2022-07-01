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

	/* TODO: print the random values
	 */

	return 0;
}
