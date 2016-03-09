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

#define FILE_NO 1

long long timer[FILE_NO];
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


/*****************************************************************************************************/
int main(int argc, char** argv)
{
	long long i=0;
	char fid[4]; 
	long long in, N;
	long long num=0;
	char *path = (char *) malloc(20000);
	path = strcat(path, "/media/quad/mybtrfs/");

	clock_init();
	
	uint64_t fc = 0, fr = 0, tr = 0;
	uint64_t lc = 0, lr = 0;

	/*******************************************************************/			
	printf("How many recurisve directories should I make master?\n");
	scanf("%lld", &N);

	char *touch = (char *) malloc(20000);
	memset(touch, '\0', 20000);

	touch = strcat(touch, "mkdir -p ");

	for(i=0; i<N; i++) // CREATE_FILES_LOOP
	{
		//sprintf(fid, "%lld", i);
		path = strcat(path, "1");
		path = strcat(path, "/");
	}
	
	touch = strcat(touch, path);
	//puts(touch);
	system(touch);
	
	memset(touch, '\0', 20000);
	touch = strcat(touch, "touch ");
	touch = strcat(touch, path);
	touch = strcat(touch, "quad");
	//puts(touch);
	system(touch);
	
	memset(touch, '\0', 20000);
	touch = strcat(touch, "echo quadcores > ");
	touch = strcat(touch, path);
	touch = strcat(touch, "quad");
	//puts(touch);
	system(touch);

	memset(touch, '\0', 20000);
	touch = strcat(touch, "cat ");
	touch = strcat(touch, path);
	touch = strcat(touch, "quad");
	puts(touch);
	
	system("/home/quad/CBShome/quad_scripts/remount.sh");
	system("echo 3 > /proc/sys/vm/drop_caches");

	fr = gethrtime(); // Start time
	system(touch);
	lr = gethrtime(); // End time
	tr = (lr-fr);

	printf("\n __________________________________");
	printf("\n| File_IO | 	  Time (us)  	");
	printf("\n|_________|________________________\n");
	printf("| LR    |       %" PRIu64 "\n", lr);
	printf("| FR    |       %" PRIu64 "\n", fr);
	printf("| READ    |       %" PRIu64 "\n", tr);
	printf("|_________|________________________\n");
	
	free(touch);
	free(path);
	return 0;
}