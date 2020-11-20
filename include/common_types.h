#pragma once
#pragma once
#include <stdbool.h>

typedef void* Pointer;
typedef char* String;
typedef unsigned int uint;
// Pointer to a function that compares 2 items "a" and "b" and returns the following values:
// < 0 if a < b
// = 0 if a = b 
// > 0  if a > b 
typedef int (*CompareFunc)(Pointer a, Pointer b); 

// Pointer to a function that destroys an element "value"
typedef void (*DestroyFunc)(Pointer value);


// Struct to hold prime numbers and time that took to find
struct info {
        int num;
        double timeneeded;
} ;

typedef struct info* Info;
