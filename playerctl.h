#ifndef PLAYERCTL_H
#define PLAYERCTL_H

#include <stdbool.h>

static const char* FORMAT_TITLE = "{{ title }}";
static const char* FORMAT_ARTIST = "{{ artist }}";
static const char* FORMAT_PLAYBACK_STATUS = "{{ status }}";
static const char* FORMAT_ART_URL = "{{ mpris:artUrl }}";

void playerctl_next();
void playerctl_previous();
void playerctl_play_pause();

static void playerctl_get_info(char const* format, 
        char* output);
static void playerctl_notify();
static void remove_newline_at_end(char* str);
static void playerctl_change_and_notify(const char* command);
static void playerctl_change_and_notify_new_thread(
        const char* command);

#endif
