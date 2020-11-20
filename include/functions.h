#pragma once
#include "common_types.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

//This function prints the text in "sentence" in red color 
void printRed(const char* sentence);

//This function prints the text in "sentence" in blue color 
void printBlue(const char* sentence);

//This function prints the text in "sentence" in blue color 
void printGreen(const char* sentence);

//This function prints the text in "sentence" in yellow color 
void printYellow(const char* sentence);

// This function is used first to check if the 7 command line arguments are 
// in correct sequence and then to ascribe their values to the main program 
// variables
void loadArgs(String argv[], int *lb, int *ub , int *chN);

// this function is used to compare info structs
int compare_infos(Pointer a, Pointer b);

// this function is used to create an array of pipes
void createPipes(int ***pipeArr,int workers);

// this fucntion is used to destroy an array of pipes
void destroyPipes(int **pipeArr,int workers);