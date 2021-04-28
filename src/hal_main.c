#include <stdio.h>
#include <raylib.h>

#include "app/app.h"
#include "app/scene.h"

#include "mod_play.h"

int main()
{
    // initialization (no touchy!!!!)
    SetTraceLogLevel(LOG_FATAL);
    AppInit("YO YO YO!", 2);

    // register scenes here
    RegisterScene(0, &PlayExport);

    // main loop
    AppMain();

    // cleanup
    UnloadScene(); // unload loaded scene at end of program
    AppCleanup();
    return 0;
}