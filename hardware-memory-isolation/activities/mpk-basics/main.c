#include <sys/mman.h>

#include <string.h>
#include <errno.h>
#include <stdio.h>

#define STRING "Hello MPK\0"
#define STRING_LEN 10

#define PKEY_DISABLE_ACCESS    0x1
#define PKEY_DISABLE_WRITE     0x2

int main()
{
    int ret, pkey;
    char *string = mmap(0, STRING_LEN, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, 0, 0);

    if(!string)
    {
        fprintf(stderr, "mmap failed: %s\n", strerror(errno));
        return 1;
    }
    memcpy(string, STRING, STRING_LEN);

    printf("%s\n", string);

    ret = mprotect(string, STRING_LEN, PROT_READ);
    if(ret != 0)
    {
        fprintf(stderr, "mprotect failed: %s\n", strerror(errno));
        return 1;
    }

    // segfault 
    // printf("%s\n", string);

    pkey = pkey_alloc(0, PKEY_DISABLE_ACCESS);
    if(pkey == -1)
    {
        fprintf(stderr, "pkey_alloc failed: %s\n", strerror(errno));
        return 1;
    }

    printf("pkey: %d\n", pkey);

    ret = pkey_mprotect(string, STRING_LEN, PROT_READ | PROT_WRITE, pkey);
    if(ret != 0)
    {
        fprintf(stderr, "pkey_mprotect failed: %s\n", strerror(errno));
        return 1;
    }

    printf("%s\n", string);

    munmap(string, STRING_LEN);
    pkey_free(pkey);

    return 0;
}

