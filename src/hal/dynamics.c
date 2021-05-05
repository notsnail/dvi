#include "dynamics.h"

#include <stdio.h>

#ifndef _WIN32 // definitions for unix systems
#include <dlfcn.h>

Dynamic LoadDynamic(const char* path)
{
    Dynamic result = dlopen(path, RTLD_LAZY);
    if (!result)
    {
        printf("failed loading dynamic %s:\n%s\n", path, dlerror());
        return NULL;
    }

    return result;
}

void UnloadDynamic(Dynamic dynamic)
{
    if (dlclose(dynamic) != 0)
    {
        printf("failed to close a dynamic.\n");
        return;
    }
}

void* GetDynamicSymbol(Dynamic dynamic, const char* sym_name)
{
    void* result = dlsym(dynamic, sym_name);
    if (!result)
    {
        printf("failed retrieving symbol %s from dynamic:\n%s\n", sym_name, dlerror());
        return NULL;
    }

    return result;
}
#endif