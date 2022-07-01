#include <stdio.h>
#include <time.h>
#include <immintrin.h>

int main(void)
{
	unsigned int rand_nr;
	FILE *urand;
	clock_t start, end;

	// Read 10k numbers from /dev/urandom
	start = clock();
	urand = fopen("/dev/urandom", "r");
	for (int i = 0; i < 10000; i++)
		fread(&rand_nr, 4, 1, urand);
	fclose(urand);
	end = clock();
	printf("Reading 10k numbers from /dev/urandom: %lfs\n", (double)(end - start) / CLOCKS_PER_SEC);

	// Read 10k numbers from rdrand
	start = clock();
	for (int i = 0; i < 10000; i++)
		_rdrand32_step(&rand_nr);
	end = clock();
	printf("Reading 10k numbers from RDRAND: %lfs\n", (double)(end - start) / CLOCKS_PER_SEC);

	// Read 10k numbers from rdseed
	start = clock();
	for (int i = 0; i < 10000; i++)
		_rdseed32_step(&rand_nr);
	end = clock();
	printf("Reading 10k numbers from RDSEED: %lfs\n", (double)(end - start) / CLOCKS_PER_SEC);

	return 0;
}
