#ifndef HAL_FS_H
#define HAL_FS_H

#include <stdbool.h>

extern void FSOpen(const char* root);
extern void FSClose();

extern void FSMount(const char* path, bool is_searched_first); // package stuff

extern bool FSFileExists(const char* url);
extern void* FSLoadFile(const char* url); // data must be freed afterwards

#endif