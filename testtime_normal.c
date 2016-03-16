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
#include "timecalc.h"

#define FILE_NO 1

char *path = "/media/quad/mybtrfs/";
long long timer[FILE_NO];

/*****************************************************************************************************/
int main(int argc, char** argv)
{
	long long i=0;
	char fid[128]; 
	long long in, N;
	long long num=0;
	
	clock_init();
	
	uint64_t fc = 0, fr = 0, tr = 0;
	uint64_t lc = 0, lr = 0;

	/*******************************************************************/			
	N = 100;
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

	for(i=0; i<N; i++) // READ_FILES_LOOP
	{
		char *temp = (char *) malloc(100);
		memset(temp, '\0', 100);
		
		if(i % 1 == 0)
			system("echo 3 > /proc/sys/vm/drop_caches");
		
		sprintf(fid, "%lld", i);

		temp = strcat(temp, "open ");
		temp = strcat(temp, path);
		temp = strcat(temp, fid);

		/*****************************/
		  fr = gethrtime(); // Start
		  system(temp);
	 	  lr = gethrtime(); // End
		/*****************************/
				
		if(lr > fr) {
			num = (lr-fr); // Add time
			printf("%lld -> %lld\n", i, num);
		}
		
		tr += num;
	}
	
	printf("\n _______________________________");
	printf("\n| File_IO | 	  Time (us)  	|");
	printf("\n|_________|_____________________|\n");
	printf("| READ    |       %" PRIu64 "\n", tr);
	printf("|_________|_____________________|\n");
	
}