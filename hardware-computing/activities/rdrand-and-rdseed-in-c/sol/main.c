extern void get_rdrand(void);
extern void get_rdseed(void);

int main(void)
{
	get_rdrand();
	get_rdseed();
}
