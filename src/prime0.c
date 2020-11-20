
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

unsigned long modular_pow (unsigned long base,unsigned long exponent,unsigned long modulus)
{
    unsigned long result;
    
    if (modulus ==1)
        return 0;
    
    result=1;
    base=base%modulus;
    while (exponent>0) {
        if (exponent%2==1)
            result = (result*base) % modulus;
        
        exponent=exponent>>1;
        base= (base*base) % modulus;
        
    }
    
    return result;
    
}

///Here is a function for a.To return a=1||a=7||a=61.
int choose(int x)
{
    
    int a;
    
    if (x==1)
        a = 2;
    else if (x==2)
        a = 7;
    else if (x==3)
        a = 61;
    
    return a;
}


//MILER-RABIN PSEUDOCODE IN C ALOGORITHM

int MilerRabin (int s ) {
    if (s == 2 || s == 7 || s ==61 ) return YES;
    unsigned long  x,q,a,d ;
    int w,flag,r,isprime,p=0;
    
        d = s - 1;
        r = 0;
        
        while (d % 2 == 0 ){
            d = d/2;
            r++;
        } //Inside this "while" loop , we just get the number in factors of 2^r * (an odd number "d")
        
        isprime = 1;
        for(int i  = 1; i <= 3; i++){ //Actually "i" is the sequence of a's we use (we use 3 a's :a=2|7|61 )
            flag = 0;
            a = choose(i); // "a" gets the value of the function "choose(i)"
            
            x = modular_pow (a,d,s); //x gets the value of the function "modular_pow", x=(a^d)%s.
            
            if ((x==1) || (x==s-1))
                continue; //In case of x=1 or x=s-1 , we need to check for next a.
            
            for(w=1; (w<=(r-1)); w++){ //else in case of x!1 or x!s-1
                
                x = modular_pow(x,2,s); //"x" gets the value of "modular_pow" function, x=(x^2)%s
                
                if (x==s-1){
                    flag=1;
                    break; //In case of the "if" statement is true, break the 2nd "for" loop and check for the next a, if it exists.
                }
            }
            
            if (flag)
                continue; //check for next a,if exists.
            else{
                isprime = 0; //i isn't prime
                break; //stop checking for this i.
            }
            
        }
        
        if (isprime)
            return YES;
    
    return NO;
    
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
                if ( MilerRabin(i)==YES ){
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