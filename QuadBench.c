/*
 * 	Normal FS
 */

#include <stdio.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include <libgen.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

/*
 * Lets us use the rdtsc instruction to get high_res time.
 * Thanks to libmicro
 */
uint64_t cpu_hz = 0;
uint64_t hrtime_t;

/*
 * Uses the rdtsc instruction to get high resolution (cpu
 * clock ticks) time. Only used for non Sun compiles.
 */
__inline__  unsigned long long
rdtsc(void)
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

uint64_t
gethrtime(void)
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
static uint64_t
parse_cpu_hz(void)
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
void
clock_init(void)
{
	cpu_hz = parse_cpu_hz();
	if (cpu_hz <= 0) {
		printf("\nClock init ERROR\n");
	}
}


/*****************************************************************************************************/
int 
main(int argc, char* argv)
{
	long long i=0;
	char fid[128]; 
	long long in;
	char *path = NULL;
	path = "/home/quad/Benchmark/temp/";
	
	system("mkdir /home/quad/Benchmark/temp"); // Make temporary Directory
	
	char *rubbish = NULL;
	rubbish = "/home/quad/Desktop/Benchmark/temp//home/quad/Desktop/Benchmark/temp//home/quad/Desktop/Benchmark/temp/\
		   /home/quad/Desktop/Benchmark/temp//home/quad/Desktop/Benchmark/temp//home/quad/Desktop/Benchmark/temp/\
		   /home/quad/Desktop/Benchmark/temp//home/quad/Desktop/Benchmark/temp//home/quad/Desktop/Benchmark/temp/";
	
	
	int len_buf = sizeof(rubbish);
	
	clock_init();
	
	uint64_t fc = 0, fr = 0;
	uint64_t lc = 0, lr = 0;

	/*******************************************************************/			
	fc = gethrtime(); // START_CREATE
	for(i=0; i<5000; i++) // CREATE_FILES_LOOP
	{
		char *temp = (char *) malloc(50);
		temp = strcpy(temp, path);
		sprintf(fid, "%lld", i);
		temp = strcat(temp, fid);
		
		FILE* fp = fopen(temp, "wb"); // OPEN
		fprintf(fp, "%lld", i); // WRITE		
		fclose(fp);
		
		memset(fid, '0', strlen(fid));
		free(temp);
	}
	lc = gethrtime(); // END_CREATE

	/*******************************************************************/	
	fr = gethrtime(); // START_READ
	for(i=0; i<5000; i++) // READ_FILES_LOOP
	{
		char *temp = (char *) malloc(1000);
		temp = strcpy(temp, path);
		sprintf(fid, "%lld", i);
		temp = strcat(temp, fid);

		FILE* fp = fopen(temp, "rb"); // OPEN
		fscanf(fp, "%lld", &in); // WRITE
		fclose(fp);
		
		memset(fid, '0', strlen(fid));
		free(temp);
	}
	lr = gethrtime(); // END_READ
		
	printf("\n _______________________________");
	printf("\n| File_IO | 	  Time (us)  	|");
	printf("\n|_________|_____________________|\n");
	printf("| CREATE  |       %-13ld |\n", (lc-fc)/1000000);	
	printf("| READ    |       %-13ld |\n", (lr-fr)/1000000);
	printf("|_________|_____________________|\n");
	
	sleep(2);
//	system("rm -r /home/quad/Benchmark/temp/"); // Remove temporary Directory	
}



