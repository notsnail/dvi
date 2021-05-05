#include "util.h"

int hal_stoi(const char* str)
{
    int val = 0;
    while (*str) {
        val = val * 10 + (*str++ - '0');
    }
    return val;
}