#include "playerctl.h"
#include <stdlib.h>
#include <pthread.h>

void playerctl_next() {
    system("playerctl next");
}

void playerctl_previous() {
    system("playerctl previous");
}

void playerctl_play_pause() {
    system("playerctl play-pause");
}

