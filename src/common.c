#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *
AllocateStringFromRange(char *Start, size_t Length)
{
    char *Result = 0;

    Result = malloc(Length + 1);
    memcpy(Result, Start, Length);
    Result[Length] = 0;

    return Result;
}

char *
AllocateStringCopy(char *String, size_t Length)
{
    char *Result = 0;

    Result = malloc(Length + 1);
    memcpy(Result, String, Length);
    Result[Length] = 0;

    return Result;
}

read_file_result
ReadEntireFileIntoMemoryAndNullTerminate(char *FileName)
{
    read_file_result Result = { 0 };

    FILE *File = fopen(FileName, "r");
    if(File)
    {
        fseek(File, 0, SEEK_END);
        Result.Length = ftell(File);
        fseek(File, 0, SEEK_SET);

        Result.Data = (char *)malloc(Result.Length + 1);
        fread(Result.Data, Result.Length, 1, File);
        Result.Data[Result.Length] = 0;

        fclose(File);
    }

    return(Result);
}