#ifndef FILE_H
#define FILE_H

struct file_t
{
    char Path[1024];
    struct file_t *Next;
};
typedef struct file_t file;

file *ListFiles(char *Path);

#endif // FILE_H