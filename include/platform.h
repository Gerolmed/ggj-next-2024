#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include "include/types.h"
#include "include/arena.h"

// content of dest is null terminated
char* read_file(const char* file, i32* flen, Arena* arena);

#endif
