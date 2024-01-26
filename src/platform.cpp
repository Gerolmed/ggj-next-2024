#include "include/platform.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// the flen-th byte is 0
char* read_file(const char* file, i32* flen, Arena* arena)
{
    FILE* fptr = fopen(file, "rb");
    if (fptr == NULL) {
        printf("Failed to read file: %s\n", file);
        return NULL;
    }
    fseek(fptr, 0, SEEK_END);
    i32 len = ftell(fptr);
    char* buf = NULL;
    if (arena) {
        buf = (char*) push_size(arena, len + 1);
    } else {
        buf = (char*) malloc(len + 1);
    }
    fseek(fptr, 0, SEEK_SET);
    fread(buf, len, 1, fptr);
    buf[len] = 0;
    if (flen)
        *flen = len;
    fclose(fptr);
    return buf;
}

