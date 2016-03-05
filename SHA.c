#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include <libgen.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include "sha256.c"

#define SHA256_DIGEST_LENGTH 64

int main ()
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    int i;

    calc_sha256("/home/quad/Benchmark/hi", hash);

    for(i=0; i<32; i++)
    {
        printf("%02x", hash[i]);
    }
}

int calc_sha256 (char* path, unsignedchar hash[SHA256_DIGEST_LENGTH])
{
    int i;

    FILE* file = fopen(path, "rb");
    if(!file) 
        return -1;

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    const int bufSize = 32768;
    unsigned char* buffer = malloc(bufSize);
    int bytesRead = 0;
    if(!buffer) 
        return -1;

    while((bytesRead = fread(buffer, 1, bufSize, file)))
    {
        SHA256_Update(&sha256, buffer, bytesRead);
    }
    SHA256_Final(hash, &sha256);

    for(i=0; i<32; i++)
    {
        printf("%02x", hash[i]);
    }

    fclose(file);
    free(buffer);

    printf("\n");
    return 0;
}      
