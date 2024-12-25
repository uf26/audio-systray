#include "notify.h"
#include "playerctl.h"
#include <stdlib.h>
#include "pulse_audio_actions.h"
#include <pthread.h>

void playerctl_next() {
    playerctl_change_and_notify("playerctl next");
}

void playerctl_previous() {
    playerctl_change_and_notify("playerctl previous");
}

void playerctl_play_pause() {
    playerctl_change_and_notify("playerctl play-pause");
}

static void playerctl_change_and_notify_new_thread(
        const char* command) {
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread, &attr, 
            (void*(*)(void*))playerctl_change_and_notify, 
            (void*) command);
}
    
static void playerctl_change_and_notify(const char* command) {
    system(command);

    usleep(500 * 1000);

    playerctl_notify();
}

static void playerctl_get_info(char const* format, 
        char* output) {
    string command;
    sprintf(command, "playerctl metadata --format='%s'", format);

    FILE* fp = popen(command, "r");
    if (fp == NULL) {
        *output = '\0';
        return;
    }

    fgets(output, MAX_STR_LEN, fp);
    remove_newline_at_end(output);

    pclose(fp);
}

static void playerctl_notify() {
    string title, artist, icon;
    bool playing;

    playerctl_get_info(FORMAT_TITLE, title);
    playerctl_get_info(FORMAT_ARTIST, artist);
    playerctl_get_info(FORMAT_ART_URL, icon);

    string playing_str;
    playerctl_get_info(FORMAT_PLAYBACK_STATUS, playing_str);
    playing = strcmp(playing_str, "Playing") == 0;

    notify_playerctl_change(title, artist, icon, playing);
}

static void remove_newline_at_end(char* str) {
    if (*str == '\0')
        return;

    while (*(str + 1) != '\0')
        str++;

    if (*str == '\n')
        *str = '\0';
}
