#include <stdio.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include <libgen.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "sha256.c"

#define SHA256_DIGEST_LENGTH 64
#define FILE_NO 4000

unsigned char* hash_tab[FILE_NO];
void calc_hash(int N);
char *path = "/media/quad/mybtrfs/";

uint64_t cpu_hz = 0;
uint64_t hrtime_t;

/*
 * Uses the rdtsc instruction to get high resolution (cpu
 * clock ticks) time. Only used for non Sun compiles.
 */
__inline__  unsigned long long rdtsc(void)
{
	unsigned long long x;
	__asm__ volatile(".byte 0x0f, 0x31" : "=A" (x));
	return (x);
}

/*
 * Get high resolution time in nanoseconds. This is the version
 * used when NOT compiled for Sun systems. It uses rdtsc call to
 * get clock ticks and converts to nanoseconds
 */

uint64_t gethrtime(void)
{
	uint64_t hrt;

	/* convert to nanosecs and return */
	hrt = ((rdtsc() * ((double)1000000UL / cpu_hz)));
	return (hrt);
}

/*
 * Gets CPU clock frequency in MHz from cpuinfo file.
 * Converts to cpu_hz and stores in cpu_hz global uint64_t.
 * Only used for Non-Sun compiles.
 */
static uint64_t parse_cpu_hz(void)
{
	/*
	 * Parse the following from /proc/cpuinfo.
	 * cpu MHz		: 2191.563
	 */
	FILE *cpuinfo;
	double hertz = -1;
	uint64_t hz;

	if ((cpuinfo = fopen("/proc/cpuinfo", "r")) == NULL) {
		printf("\nCPU_HZ_Parse failed\n");
	}
	while (!feof(cpuinfo)) {
		char buffer[80];

		fgets(buffer, 80, cpuinfo);
		if (strlen(buffer) == 0) continue;
		if (strncasecmp(buffer, "cpu MHz", 7) == 0) {
			char *token = strtok(buffer, ":");

			if (token != NULL) {
				token = strtok((char *)NULL, ":");
				hertz = strtod(token, NULL);
			}
			break;
		}
	}
	hz = hertz * 1000000;

	return (hz);
}

/*
 * Sets the cpu clock frequency variable or shuts down the
 * run if one is not found.
 */
void clock_init(void)
{
	cpu_hz = parse_cpu_hz();
	if (cpu_hz <= 0) {
		printf("\nClock init ERROR\n");
	}
}

void init_hashtab()
{
	int i;
	
	for(i=0; i<FILE_NO; i++)
	{
		hash_tab[i] = (unsigned char *)malloc(40 * sizeof(unsigned char));
		memset(hash_tab[i], '\0', 40);
	}
}

int calc_sha256 (char* path, unsigned char* hash)
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

    fclose(file);
    free(buffer);
    return 0;
} 

void print_hash(unsigned char hash[])
{
   	int i;
  
   	printf(" ");
   	for(i=0; i<32; i++)
   	{
    	   	printf("%02x", hash[i]);
    	}
   
   	printf("\n");
}

void calc_hash(int N)
{
    int i, ret;

    for(i=0; i<N; i++)
    {
		char *temp = (char *) malloc(50);
		char fid[128]; 	
		int ret, idx, j=0;
		int c;
		
		temp = strcpy(temp, path);
		sprintf(fid, "%d", i);
		temp = strcat(temp, fid);

		ret = calc_sha256(temp, hash_tab[i]);

    }
}

void fix_hash(int N)
{
	int i;
	unsigned char *final = (unsigned char *) malloc(100);
	memset(final, '\0', 100);
	unsigned char *foo;
	unsigned char *pch, *pch2;

	for(i=0; i<N; i++)
	{
		memset(final, '\0', 100);
		foo = NULL;
		pch = NULL;
		pch2 = NULL;

		pch=strchr(hash_tab[i],'/');
		pch2=strchr(hash_tab[i],'\'');
	  	if(pch!=NULL || pch2!=NULL)
	  	{
	  		//printf("Ghaan vala fix :D\n");
	    	memcpy(hash_tab[i],hash_tab[3],35);
	  	}
    }
}

/*****************************************************************************************************/
int main(int argc, char** argv)
{
	long long i=0, o;
	char fid[128]; 
	long long in, N;
	long long num=0;

	clock_init();
	
	struct timeval start, end;
	uint64_t fc = 0, fr = 0, tr = 0;
	uint64_t lc = 0, lr = 0;

	/*******************************************************************/				
	system("/home/quad/formatwithcbs.sh");
	printf("\n\n=============================== Benchmarking for CONTENT BASED STORAGE =============================== \n\n");	
	printf("Number of files to create \n");
	scanf("%lld", &N);
	printf("Here I go!\n");
	
	for(i=0; i<N; i++) // CREATE_FILES_LOOP
	{
		char *touch = (char *) malloc(100);
		char *echo = (char *) malloc(100);
		memset(touch, '\0', 100);
		memset(echo, '\0', 100);

		touch = strcat(touch, "touch ");
		touch = strcat(touch, path);
		sprintf(fid, "%lld", i);
		touch = strcat(touch, fid);

		system(touch);

		free(touch);

		echo = strcat(echo, "echo ");
		echo = strcat(echo, fid);
		echo = strcat(echo, " > ");
		echo = strcat(echo, path);
		echo = strcat(echo, fid);

		system(echo);

		free(echo);
	}

	/*******************************************************************/
	init_hashtab();
	calc_hash(N);
	fix_hash(N);

	system("umount -f /media/quad/mybtrfs/");
	system("mount /dev/sda4 /media/quad/mybtrfs");
	system("sleep 1");
	printf("REMOUNTED BTRFS TO DROP CACHES\n");
	/*******************************************************************/	

	system("sync");

	for(i=0; i<N; i++) // READ_FILES_LOOP
	{
		char *temp = (char *) malloc(100);
		memset(temp, '\0', 100);
		
		/*if(i % 1 == 0)
			system("echo 3 > /proc/sys/vm/drop_caches");*/
		
		temp = strcat(temp, path);
		if(strlen(hash_tab[i]) == 32)
			temp = strcat(temp, hash_tab[i]);
		else
			temp = strcat(temp, hash_tab[1]);

		//puts(temp);

		gettimeofday(&start, NULL);
		o = open(temp, O_RDWR);
		gettimeofday(&end, NULL);
		
		//printf("\nOpen returns %lld.\n", o);
		close(o);

		free(temp);
		if((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) 
		{
			num = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)); // Add time
			//printf("%llu -> %llu\n", i, num);
			tr += num; // Add time
		}			
	}

	printf("\n _______________________________");
	printf("\n| File_IO | 	  Time (us)  	|");
	printf("\n|_________|_____________________|\n");
	printf("| READ    |       %" PRIu64 "\n", tr);
	printf("|_________|_____________________|\n");
	
}