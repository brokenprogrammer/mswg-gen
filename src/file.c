#include "file.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>

file * 
ListFiles(char *Path)
{
    file *FirstFile = NULL;
    file *CurrentFile = NULL;

    struct dirent *DirectoryEntry;
    DIR *Directory = opendir(Path);

    if (!Directory)
    {
        perror("Failed to open directory for file search.");
        exit(EXIT_FAILURE);
    }

    while ((DirectoryEntry = readdir(Directory)) != NULL)
    {
        if (strcmp(DirectoryEntry->d_name, ".") != 0 && 
            strcmp(DirectoryEntry->d_name, "..") != 0)
        {
            
            if (DirectoryEntry->d_type == DT_DIR)
            {
                char Buffer[4096];
                realpath(DirectoryEntry->d_name, Buffer);
                file *FileList = ListFiles(Buffer);

                if (CurrentFile == NULL)
                {
                    CurrentFile = FileList;
                    if (FirstFile == NULL)
                    {
                        FirstFile = CurrentFile;
                    }
                }
                else
                {
                    CurrentFile->Next = FileList;
                    while (CurrentFile->Next != NULL)
                    {
                        CurrentFile = CurrentFile->Next;
                    }
                }
            }
            else
            {
                if (CurrentFile == NULL)
                {
                    CurrentFile = malloc(sizeof(file));
                    if (FirstFile == NULL)
                    {
                        FirstFile = CurrentFile;
                    }

                    // realpath(DirectoryEntry->d_name, CurrentFile->Path);
                    sprintf(CurrentFile->Path, "%s/%s", Path, DirectoryEntry->d_name);
                    CurrentFile->Next = NULL;
                }
                else
                {   
                    CurrentFile->Next = malloc(sizeof(file));
                    
                    // realpath(DirectoryEntry->d_name, CurrentFile->Next->Path);
                    sprintf(CurrentFile->Next->Path, "%s/%s", Path, DirectoryEntry->d_name);
                    CurrentFile->Next->Next = NULL;
                    CurrentFile = CurrentFile->Next;
                }                
            }
        }
    }

    closedir(Directory);
    return FirstFile;
}