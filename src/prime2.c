#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include "../include/common_types.h"
#define YES 1
#define NO  0


int prime(int n){
        int i;
        if (n==1) return(NO);
        for (i=2 ; i<n ; i++)
                if ( n % i == 0) return(NO);
        return(YES);
}

int main(int argc, char *argv[]){
        
        int lb=0, ub=0, i=0;

        if ( (argc != 4) ){
                printf("usage: prime1 lb ub\n");
                exit(1); }

        lb=atoi(argv[1]);
        ub=atoi(argv[2]);
        int count = 0;
        int pos = 1 ;

        //fd for write that given as argument 
        int wrPipe = atoi(argv[3]);
        
        /* the maximum number of nums each prime* program can find is 
        upperbound-lowerbound. That is a number closer to the realistic
        number of prime numbers eac program can found. Realistic number
        is fewer than the half of the current maxNum */
        int maxNum = ub - lb;
       
        // an array of structs info 
        struct info infoArr[maxNum];

        // printf("lower bound %d upper bound %d prime 0 \n",lb,ub);
        if ( ( lb<1 )  || ( lb > ub ) ) {
                printf("usage: prime1 lb ub\n");
                exit(1); }

        double t1, t2, t3,t4;
        struct tms tb1, tb2, tb3, tb4;
        double ticspersec;
        ticspersec = (double) sysconf(_SC_CLK_TCK);
        t1 = (double) times(&tb1);
        t3 = (double) times(&tb3);
        for (i=lb ; i <= ub ; i++){     
                if ( prime(i)==YES ){
                        // printf("%d ",i);
                        t2 = (double) times(&tb2);
                        infoArr[pos].timeneeded = (t2 - t1) / ticspersec;
                        infoArr[pos].num = i;
                        pos++;
                        count++;
                }
        }
        t4 = (double) times(&tb4);
        double execTime  = (t4 - t3) / ticspersec;
        // printf("\nfound %d primes in %lf\n",count, execTime);

        /* the first struct of the array contains external info
        for the array of structs. In num field is written how many 
        prime numbers are found&written and the timeneeded field 
        describes how many time the whole program prime* took till end. */
        infoArr[0].num = count;
        infoArr[0].timeneeded =  execTime;
        
        if (write(wrPipe, infoArr, (1+count)*sizeof(struct info)) ==-1) 
                printf("error in write\n");
        close(wrPipe);

        // printf("written %d times\n",count);
        // printf("\n");
}
