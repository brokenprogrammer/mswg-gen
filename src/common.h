#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

typedef enum { false, true } bool;

typedef struct 
{
    char *Data;
    size_t Length;
} read_file_result;

char *AllocateStringFromRange(char *Start, size_t Length);
char *AllocateStringCopy(char *String, size_t Length);
read_file_result ReadEntireFileIntoMemoryAndNullTerminate(char *FileName);


#endif // COMMON_H