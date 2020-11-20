#include "../include/functions.h"



void printRed(const char* sentence){
    if(sentence != NULL){
        printf("\033[1;31m");
        printf("%s",sentence);
        printf("\033[0m");
    }
}

void printBlue(const char* sentence){
    if(sentence != NULL){
        printf("\033[0;34m");
        printf("%s",sentence);
        printf("\033[0m");
    }
}

void printGreen(const char* sentence){
    if(sentence != NULL){
        printf("\033[0;32m");
        printf("%s",sentence);
        printf("\033[0m");
    }
}

void printYellow(const char* sentence){
    if(sentence != NULL){
        printf("\033[0;33m");
        printf("%s",sentence);
        printf("\033[0m");
    }
}

void loadArgs(String argv[], int *lb, int *ub , int *chN){
    int lowerBound, upperBound, childrenNum;

    if( (strcmp(argv[1], "-l")==0) && (strcmp(argv[3], "-u")==0) && (strcmp(argv[5], "-w")==0)  ){
            lowerBound = atoi(argv[2]);
            upperBound = atoi(argv[4]);
            childrenNum = atoi(argv[6]);
    }
    else if( (strcmp(argv[1], "-l")==0) && (strcmp(argv[3], "-w")==0) && (strcmp(argv[5], "-u")==0)  ){
        lowerBound = atoi(argv[2]);
        upperBound = atoi(argv[6]);
        childrenNum = atoi(argv[4]);
    }
    else if( (strcmp(argv[1], "-u")==0) && (strcmp(argv[3], "-l")==0) && (strcmp(argv[5], "-w")==0)  ){
        lowerBound = atoi(argv[4]);
        upperBound = atoi(argv[2]);
        childrenNum = atoi(argv[6]);
    }
    else if( (strcmp(argv[1], "-u")==0) && (strcmp(argv[3], "-w")==0) && (strcmp(argv[5], "-l")==0)  ){
        lowerBound = atoi(argv[6]);
        upperBound = atoi(argv[2]);
        childrenNum = atoi(argv[4]);
    }
    else if( (strcmp(argv[1], "-w")==0) && (strcmp(argv[3], "-l")==0) && (strcmp(argv[5], "-u")==0)  ){
        lowerBound = atoi(argv[4]);
        upperBound = atoi(argv[6]);
        childrenNum = atoi(argv[2]);
    }
    else if( (strcmp(argv[1], "-w")==0) && (strcmp(argv[3], "-u")==0) && (strcmp(argv[5], "-l")==0)  ){
        lowerBound = atoi(argv[6]);
        upperBound = atoi(argv[4]);
        childrenNum = atoi(argv[2]);
    }
    else{
        printRed("Invalid command line sequence.Exiting...\n");
        exit(EXIT_FAILURE);
    }
    *ub = upperBound;
    *lb = lowerBound;
    *chN = childrenNum;


}


int compare_infos(Pointer a, Pointer b) {
    return ((Info)a)->num - ((Info)b)->num;
}

void createPipes(int ***pipeArr,int workers){
    *pipeArr = malloc( workers * sizeof(int*) );
        for (int i = 0 ; i < workers ; i++){
            (*pipeArr)[i] = malloc( 2 * sizeof(int) );
            if (pipe((*pipeArr)[i]) == -1 ){
                printf("error creating pipes\n");
            }

            else{
                // printGreen("pipe created\n");
            }
        }
}

void destroyPipes(int **pipeArr,int workers){
    for (int i = 0 ; i < workers ; i++){
        free(pipeArr[i]);
    }
    free(pipeArr);
}


