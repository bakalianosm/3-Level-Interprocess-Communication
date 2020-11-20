

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../include/linked_list.h"
#include "../include/functions.h"



 /*
  * Arguments input type is [ lower bound, upper bound, num of children - workers in this case]
  */

int main(int argc, char* argv[]){

    if (argc !=6){
        printRed("Wrong amount of arguments in Intermediate Level. Exiting ... \n");
        exit(EXIT_FAILURE);
    }

    int iLowerBound, iUpperBound, workers, numbers, fraction; // childrenNum should be renamed as workers.
    
    
    // parse the arguments as integers 
    iLowerBound = atoi(argv[1]);
    iUpperBound = atoi(argv[2]);
    int writePipe = atoi(argv[4]);
    int writeTimesPipe = atoi(argv[5]);
    workers = atoi(argv[3]);
    
    numbers = iUpperBound  - iLowerBound + 1;
    // how many numbers each [W] orker calculate
    fraction = numbers / workers ;
    if( numbers % fraction !=0 ) fraction++;
   
    /*  we took the number of the children that the indermediate level makes
     * and what we have to do now is to fork for number of children times 
     */

    pid_t child_pid; 

    int workerNum = 0 ;
    int lowerAssigned, upperAssigned ;

    /* 
     * initialize pipes for prime numbers transport IPC
     */
    int **prime_pipes= NULL;
    createPipes(&prime_pipes,workers);
    

    for (workerNum = 0 ; workerNum < workers ; workerNum ++){        
        
        /*
         * make the number ranges to be assigned at each worker
         */ 
        lowerAssigned = iLowerBound + workerNum*fraction  ;
        upperAssigned = iLowerBound + (workerNum + 1)*fraction - 1;
        if( upperAssigned > iUpperBound )  upperAssigned = iUpperBound ;
        
        char lowerAssignedString [8];
        char upperAssignedString [8];


        /*
         * forking a new process
         */ 
        child_pid = fork();
        
        // check for failure of fork
        if (child_pid < 0 ){
            printf("Error while forking the %d worker \n",workerNum);
            exit(EXIT_FAILURE);
        }
        else if(child_pid == 0){

            sprintf(lowerAssignedString,"%d",lowerAssigned);
            sprintf(upperAssignedString,"%d",upperAssigned);
            
            for(int i = 0 ; i < workers ; i++){
                if (i ==  workerNum){
                    close(prime_pipes[i][0]);
                    continue;

                }
                else{
                    close(prime_pipes[i][1]);
                    close(prime_pipes[i][0]);
                    
                }
            }
            int pipeDes = prime_pipes[workerNum][1];

            /* we have only 3 prime functions as workers .
             * if the childrenNum is greater than 3, then we have to assign the workers
             * in a circular row [ this is done using the following formula : 
             * "workers(constantly in this case 3) mod children" ]
             */
            int primeFunction = workerNum % 3 ;
        
         
            /*
             * generate the corresponding prime executable name
             */ 
            char str[2];
            char executable[] = "./prime" ;
            sprintf(str,"%d",primeFunction);
            strcat(executable, str);

            char pipeDesStr[5];
            sprintf(pipeDesStr,"%d",pipeDes);

            /*
             * Execute the corresponding executable of primes with the corresponding arguments
             */ 
            execlp(executable,executable ,lowerAssignedString, upperAssignedString, pipeDesStr, NULL);
        }

    }

    /*
     * This field executes in parent process
     */ 

    for (int i = 0 ; i < workers ; i++)
        wait(NULL);    


    if (child_pid > 0){
        /* 
        * This array of structs holds for each worker
        * one struct Info . Each struct will be used to store
        * each worker's execution time and how many primes found
        */
        struct info primeCnt[workers];
        struct info info_array[workers][numbers];
        
        /* 
        * Initialize all array values with 0
        */
        memset(info_array,0, workers*numbers*sizeof(struct info));


        /* 
        * This array hold for each worker how many info structs will store
        */

        int capacities[workers];
        double workersTime[workerNum];
        
        /* 
        * Close the pipes tha are not going to used
        */
        for(int i = 0 ; i < workers ; i++){
            close(prime_pipes[i][1]);
        }


        for(int i = 0 ; i < workers ; i++){
            /* 
            * Read the first struct from each pipe to get the info 
            * for each worker . We take for each worker and integer 
            * that represents how many prime numbers the worker found
            * and in how many seconds the worker found these numbers.
            */
            read(prime_pipes[i][0],&primeCnt[i],sizeof(struct info));
            // printf("PrimeCnt[%d] %d\t and took %lf secs\t" ,i,primeCnt[i].num,primeCnt[i].timeneeded);
            
            /* 
            * Capacity for each worker means how many data are stored 
            */
            capacities[i] = primeCnt[i].num +1;
            workersTime[i]= primeCnt[i].timeneeded;
            // printf("Capacity[%d] %d\n",i,capacities[i]);

            /* 
            * Now we know how many structs trasfered throught the 
            * pipe . 
            * Read from each [read] pipe , "capacity of worker" number of 
            * stucts and then we do not need this pipe anymore.
            */
            if(read(prime_pipes[i][0],info_array[i],capacities[i]*sizeof(struct info))==1) {
                printf("Error in read\n");
            }
            close(prime_pipes[i][0]);
        }

    
        /* 
        * Create the list and  insert the elemens in ascending order
        */
        
        List list = list_create(NULL);
        for(int i = 0 ; i< workers ; i++){
            for (int j=0 ; j < primeCnt[i].num; j++){
                // printf("num is [%d] and time is [%lf]\n",info_array[i][j].num,info_array[i][j].timeneeded);
                list_insert_ordered(list,&info_array[i][j],compare_infos);
            }
        }

        /* 
        * Make an array out of the list that we created
        * This array is a copy of the list , with one difference.
        * The array has one extra element that describes how many
        * prime numbers each intermediate level found and in how 
        * many time. The following array is ordered,too.
        */
        int size = list_size(list) + 1;
        struct info ordered_numbers[size];
        int pos = 1;

        ordered_numbers[0].num = list_size(list);
        ordered_numbers[0].timeneeded = 0.0;

        // printf("List filled with %d prime numbers\n",list_size(list));

        /* 
        * Fill the array
        */

        for(ListNode node = list_first(list) ; node!= NULL ; node = list_next(list,node)){
            struct info val = *(struct info*)list_node_value(list,node);
            ordered_numbers[pos].num = val.num;
            ordered_numbers[pos].timeneeded = val.timeneeded;
            pos++;        
        }
        
        /* 
        * Even the list is ordered the array will be filled
        * with ordered vallues.
        * Then tranfer the array through the pipe to the myprime program
        */
        write(writePipe,ordered_numbers,size*sizeof(struct info));
        write(writeTimesPipe,workersTime,workers*sizeof(double));
       
        /* 
        * This print is used to verify that i passed the correct values.
        * Actually that code block is useless.
        */

        // for (int i=1 ; i< size ; i++){

        //     int number = ordered_numbers[i].num;
        //     double timess =  ordered_numbers[i].timeneeded;
            // printf("arr[%d].num = %d\t arr[%d].timeneeded = %lf\n",i,ordered_numbers[i].num, i,ordered_numbers[i].timeneeded);

        // }

        /* 
        * Close the pipes that was used to write.
        */
        close(writePipe);
        close(writeTimesPipe);

        destroyPipes(prime_pipes,workers);
        list_destroy(list);

    }
    return 0;

}   



