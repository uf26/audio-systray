#ifndef PULSE_AUDIO_ACTIONS_H
#define PULSE_AUDIO_ACTIONS_H

#include <pulse/pulseaudio.h>
#include <stdbool.h>

#define MAX_STR_LEN 100
#define SCROLL_AMOUNT 5

typedef char string[MAX_STR_LEN];

typedef struct pa_info_list_t {
    string name;
    string id;
    bool is_default;
    struct pa_info_list_t *next;
}pa_info_list;

static pa_info_list* pa_new_info_list(const char* name,
        const char* id, bool is_default);

void open_pavucontrol();

void pa_get_default_sink_id_cb(pa_context *c, const pa_server_info *i, 
        void *userdata);
void pa_get_default_sink_id(char* default_sink_id);
void pa_set_default_sink(pa_info_list* sink);

static void pa_sinklist_cb(pa_context *c, const pa_sink_info *l, 
        int eol, void *userdata);
pa_info_list* pa_get_sinks();

void pa_cycle_sink();

static int pa_cvolume_to_int(pa_cvolume* volume);
static pa_cvolume int_to_pa_cvolume(int volume, int nr_channels);

void pa_get_sink_info_cb(pa_context *c, 
    const pa_sink_info *i, int eol, void *userdata);
pa_sink_info pa_get_sink_info();
void pa_get_volume(int* volume, bool* mute);

void pa_toggle_mute();
void pa_set_mute(bool mute);
void pa_change_volume(int volume);

void pa_get_icon_name(char* icon_name);

#endif
