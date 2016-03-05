/*
 * Btrfs
 */

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
#include "sha256.c"

#define SHA256_DIGEST_LENGTH 64
#define FILE_NO 4000

void sha256_hash_string (unsigned char hash[SHA256_DIGEST_LENGTH], char outputBuffer[]);

char hash_tab[FILE_NO][65];
unsigned char bithash[32];
void calc_hash();
char *path = "/media/quad/mybtrfs/";

uint64_t cpu_hz = 0;
uint64_t hrtime_t;

int calc_sha256 (char* path, unsigned char hash[SHA256_DIGEST_LENGTH])
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
	hrt = ((rdtsc() * ((double)1000000000UL / cpu_hz)));
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

void calc_hash()
{
    long long i;

    for(i=0; i<FILE_NO; i++)
    {
    	unsigned char hash[32];           
		char *temp = (char *) malloc(50);
		char fid[128]; 	
		int ret, idx, j=0;
		int c;
		
		temp = strcpy(temp, path);
		sprintf(fid, "%lld", i);
		temp = strcat(temp, fid);
	
		calc_sha256(temp, bithash);
		sha256_hash_string(bithash, hash_tab[i]);
		//print_hash(hash_tab[i]);
    }
}

/*****************************************************************************************************/
int main(int argc, char* argv)
{
	long long i=0;
	char fid[128]; 
	long long in;
	
	clock_init();
	
	uint64_t fc = 0, fr = 0, tr = 0;
	uint64_t lc = 0, lr = 0;

	/*******************************************************************/			
	fc = gethrtime(); // START_CREATE
	for(i=0; i<FILE_NO; i++) // CREATE_FILES_LOOP
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
		//printf("\n_1__%s___\n", touch);		

		free(touch);

		echo = strcat(echo, "echo ");
		echo = strcat(echo, fid);
		echo = strcat(echo, " > ");
		echo = strcat(echo, path);
		echo = strcat(echo, fid);

		system(echo);

		//printf("\n_2__%s___\n\n", echo);

		free(echo);
	}
	lc = gethrtime(); // END_CREATE

	/*******************************************************************/
    calc_hash();
         
	/*******************************************************************/	
	//fr = gethrtime(); // START_READ
	for(i=0; i<FILE_NO; i++) // READ_FILES_LOOP
	{
		char *temp = (char *) malloc(100);
		memset(temp, '\0', 100);
		if(i % 50 == 0)
			system("echo 3 > /proc/sys/vm/drop_caches");
		
		sprintf(fid, "%lld", i);

		temp = strcat(temp, "open ");
		temp = strcat(temp, path);
		temp = strcat(temp, fid);

		fr = gethrtime();
			system(temp);
		lr = gethrtime();
		tr += (lr-fr); // END_READ
	}
		
	printf("\n _______________________________");
	printf("\n| File_IO | 	  Time (us)  	|");
	printf("\n|_________|_____________________|\n");
//	printf("| CREATE  |       %" PRIu64 "\n", (lc-fc)/100000);
	printf("| READ    |       %" PRIu64 "\n", tr);
	printf("|_________|_____________________|\n");
	
}

void sha256_hash_string (unsigned char hash[SHA256_DIGEST_LENGTH], char outputBuffer[65])
{
    int i = 0;

    for(i = 0; i < SHA256_DIGEST_LENGTH/2; i++)
    {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }

    outputBuffer[64] = 0;
}