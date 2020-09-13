#include <stdbool.h>
#include <unistd.h>
#include "agario.h"

//start of keyboard thread
void *keyboard_routine(void *arg){
    bool *playing = (bool*) arg;
    while (*playing){
        usleep(20000);
        if (gfx_keypressed() == SDLK_ESCAPE){
            *playing = false;
        }
    }

    return NULL;
}