#include<stdio.h>
#include<string.h>
#include<stdlib.h>
unsigned char hash[5000][32];
void calc_hash();

int main()
{
    calc_hash();
}

void calc_hash()
{
    int i;
    
   for(i =0; i<5000; i++)
    {
        strcpy(hash[i],"49454bcda10e0dd4543cfa39da9615a19950570129420f956352a58780550839");
        sprintf("%s\n", hash[i]);
    }
}
