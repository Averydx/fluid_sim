// fluid_sim.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRTDBG_MAP_ALLOC

#include <iostream>
#include "environment.h"
#include <crtdbg.h>


#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600

int main(int argc,char**argv)
{
    environment* environment = new class environment();
    fluid* fluid_ptr = new fluid(1.0,100,100,600/100); 
    environment->env_init((char*)"FLUID", SCREEN_WIDTH, SCREEN_HEIGHT,fluid_ptr);
    environment->run();
    environment->close();

    _CrtDumpMemoryLeaks();
    return 0;
}

