
// the source file for the Programming Exersice 2 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for exec*
#include <sys/wait.h>
#include "../include/common_types.h"
#include "../include/functions.h"
#include "../include/linked_list.h"


int main(int argc, char* argv[]){

    int lowerBound, upperBound, childrenNum;

    if (argc==7){
        // Use the suitable function to parse the arguments and test them
        loadArgs(argv,&lowerBound,&upperBound,&childrenNum);
        // printf("lb = %d, ub = %d, w = %d \n", lowerBound, upperBound, childrenNum);
    }
    else{
        printRed("Not enough command line arguments given.Exiting...\n");
        exit(EXIT_FAILURE);
    }

    /* Now we have the values that the user given*/
     
   
  
    pid_t child_pid;
    int intermediate = 0; 

    int numsForEachIntermediate = upperBound - lowerBound +1;
    int cFraction = numsForEachIntermediate / childrenNum;
    if( numsForEachIntermediate % cFraction !=0 ) cFraction++;
    // printf("Each Intermediate node will calculate %d primes\n",cFraction);

    // printf("Nums for myprime is %d\n",numsForEachIntermediate);
    
    /*
     * create the pipes that transfer the worker times
     */

    int **myprime_pipes= NULL;
    createPipes(&myprime_pipes,childrenNum);

    int **mytime_pipes = NULL;
    createPipes(&mytime_pipes,childrenNum);

    int ilb,iub;
    for (intermediate = 0 ; intermediate < childrenNum; intermediate++){
        /*
         * make the number ranges to be assigned at each intermediate 
         */ 
        ilb = lowerBound + intermediate*cFraction  ;
        iub = lowerBound + (intermediate + 1)*cFraction - 1;
        if( iub > upperBound)  iub = upperBound ;
        
        char ilbString [8];
        char iubString [8];
        char childrenNumString[5];

        /*
         * forking a new process
         */ 

        child_pid = fork();

        if(child_pid < 0){
            printf("Something went wrong from myprime in %dth fork\n",intermediate);
        }
        else if (child_pid == 0 ){

            for(int i = 0 ; i < childrenNum ; i++){
                if (i ==  intermediate){
                    close(myprime_pipes[i][0]);
                    close(mytime_pipes[i][0]);
                    continue;

                }
                else{
                   
                    close(myprime_pipes[i][1]);
                    close(myprime_pipes[i][0]);
                    close(mytime_pipes[i][1]);
                    close(mytime_pipes[i][0]);
                    
                }
            }
            // printf("passing [%d][1]\n",intermediate);
            int IpipeDes = myprime_pipes[intermediate][1];
            int ItimepipeDes = mytime_pipes[intermediate][1];

            char IpipeDesStr[5];
            char ItimepipeDesStr[5];

            sprintf(IpipeDesStr,"%d",IpipeDes);
            sprintf(ItimepipeDesStr,"%d",ItimepipeDes);
          
            // printf("Intermediate %d range [%d,%d]\n",intermediate, ilb, iub);

            
            sprintf(ilbString,"%d",ilb);
            sprintf(iubString,"%d",iub);
            sprintf(childrenNumString,"%d",childrenNum);
            // printf("Goind to call exec in myprime\n");
           
            /*
             * Execute the  executable of the intermediate level
             */ 
            execlp("./intermediate", "./intermediate",ilbString,iubString,childrenNumString,IpipeDesStr,ItimepipeDesStr,NULL);
        }   
          
         
    }
    // printf("Program about to leave\n");

    /*
     * This field executes in parent process
     */

    for(int i = 0 ; i < childrenNum ; i++)
        wait(NULL);

    for(int i = 0 ; i < childrenNum ; i++){
        close(myprime_pipes[i][1]);
        close(mytime_pipes[i][1]);
    }


    /*
     * This array is used to parse how many numbers
     * each intermediate level found. We initialize all its
     * values to 0 using memset /
     */
    struct info countForChildren[childrenNum];
    memset(countForChildren,0, childrenNum*sizeof(struct info));

    /*
     * This array is used to parse how all the prime numbers
     * each intermediate level found. We initialize all its
     * values to 0 using memset 
     */
    struct info final_numbers[childrenNum][numsForEachIntermediate];
    memset(final_numbers,0, childrenNum*numsForEachIntermediate*sizeof(struct info));
    
    /*
     * This array is used to parse how many time
     * each intermediate level needed to find the primes.
     */

    double times[childrenNum][childrenNum];

    if (child_pid>0){
        for(int i = 0 ; i < childrenNum ; i++){
                    
            read(myprime_pipes[i][0],&countForChildren[i],sizeof(struct info));
            // printf("In my prime i got is %d\n",countForChildren[i].num);
            
            read(mytime_pipes[i][0], &times[i],childrenNum*sizeof(double));

            if(read(myprime_pipes[i][0],final_numbers[i],countForChildren[i].num *sizeof(struct info))==1) {
                printf("Error in read\n");
            }
            close(myprime_pipes[i][0]);
            close(mytime_pipes[i][0]);
        }
        wait(NULL);
    }
    printf("Primes in [%d,%d] are :\n",lowerBound,upperBound);


    /*
     * Initialize the list that will store all the prime
     * numbers that found during the execution .
     * All the numbers from all levels!
     */
    List mainList = list_create(NULL);
    for(int i=0 ; i< childrenNum ; i++){
        for (int j = 0 ; j < countForChildren[i].num ; j++){
            list_insert_ordered(mainList,&final_numbers[i][j],compare_infos);
        }
    }

    // printf("Program found %d primes\n",list_size(mainList));

    
    for(ListNode node = list_first(mainList) ; node!= NULL ; node = list_next(mainList,node)){
        struct info val = *(struct info*)list_node_value(mainList,node);
        // printf("%d %lf  ",val.num,val.timeneeded);
    }
    printf("\n");

    /*
     * This code block is used to print the prefered output that
     * described at the professor's website :
     * https://www.alexdelis.eu/k22/formatted-output.f20-prj2-v1.txt
     */
    int count = 0;
    int arraysize = childrenNum*childrenNum;

    double workerTimes[arraysize];
    double minTime = 0.0;
    double maxTime = 0.0;

    for(int i=0; i<childrenNum; i++){
        for(int j= 0 ; j<childrenNum; j++){
            workerTimes[count]=times[i][j];
            count++;
        }
    }

    for(int i=0 ; i<count ;i++){
        if(workerTimes[i] > maxTime)
            maxTime = workerTimes[i];
        
        if(workerTimes[i] < minTime)
            minTime = workerTimes[i];
    }
    printf("Min Time For Workers : %lf secs\n",minTime);
    printf("Max Time For Workers : %lf secs\n",maxTime);
    printf("Num of USR1 Recieved : NULL\n");
    for(int i=0 ; i<count ;i++)
        printf("Time for W%d: %lf msecs\n",i,workerTimes[i]);


    /*
     * Deallocate all the manually allocated memory.
     */
    destroyPipes(myprime_pipes,childrenNum);
    destroyPipes(mytime_pipes,childrenNum);
    list_destroy(mainList);
    return 0 ;
}



 