#include "fs.h"

#include "error.h"
#include "../vendor/physfs.h"

#include <stdlib.h>
#include <raylib.h>

#define ERR_CODE_FAILED_TO_MOUNT -1212

void FSOpen(const char* root)
{
    if (PHYSFS_init(root) == 0)
    {
        printf("physfs no worky!\n");
    }
}

void FSClose()
{
    int result = PHYSFS_deinit();

    if (result == 0)
    {
        printf("fs failure: something is still open on exit...\n");
    }
}

void FSMount(const char* path, bool is_searched_first)
{
    if (PHYSFS_mount(path, NULL, (int) is_searched_first) == 0)
    {
        FatalErrorWithCode(TextFormat("failed to mount %s", path), ERR_CODE_FAILED_TO_MOUNT);
    }

}

bool FSFileExists(const char* url)
{

}

void* FSLoadFile(const char* url)
{
    
}

