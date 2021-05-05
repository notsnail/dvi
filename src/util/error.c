#include "error.h"

#include <stdio.h>
#include <stdlib.h>

#include "../app/app.h"
#include "../app/scene.h"
#include "../vendor/tinyfiledialogs.h"

#define NO_COCONUT -69420

void ReportErrorToWindow(const char* why, ReportErrorLevel type)
{
    switch (type)
    {
        case HALCYON_INFO: 
            tinyfd_messageBox("letting ya know", why, "ok", "info", 1);
            break;
        default:
        case HALCYON_WARNING: 
            tinyfd_messageBox("close one", why, "warning", "info", 1);
            break;
        case HALCYON_ERROR:
            tinyfd_messageBox("bruh!", why, "ok", "error", 1);
            BindScene(-1);
            break;
    }
}

void FatalErrorWithCode(const char* why, int code)
{
    tinyfd_messageBox("bruh!", why, "ok", "error", 1);
    UnloadScene();
    AppCleanup();
    exit(code);
}

void TheGameIsLackingCoconutError()
{
    tinyfd_messageBox(
        "error -69420 no coconut", 
        "where da coconut at doe????", 
        "ok",
        "error",
        1
    );

    UnloadScene();
    exit(NO_COCONUT);
}