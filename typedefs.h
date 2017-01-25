#ifndef TYPEDEFS
#define TYPEDEFS

// typedefs //

#ifdef PERFORM_CALCULATIONS
typedef float binType;
#else
typedef int binType;
#endif

typedef int ctrType;

typedef size_t sizeType;

//the output type of the encryption algorithm used
// (to create appropriate type of array)
typedef unsigned long encrType; 

#endif
