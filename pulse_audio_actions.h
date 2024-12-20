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
    pa_cvolume volume;
    bool mute;
    struct pa_info_list_t *next;
}pa_info_list;

static pa_info_list* pa_new_info_list(const char* name,
        const char* id, bool is_default,
        pa_cvolume volume, bool mute);

void open_pavucontrol();

void pa_free_sinks();

static void pa_update_sinks_sinklist_cb(
        pa_context *c, const pa_sink_info *l, 
        int eol, void *userdata);

static pa_info_list* pa_get_sink_by_id(
        const char* id);

static void pa_update_sinks_get_default_sink_cb(
        pa_context *c, const pa_server_info *i, 
        void *userdata);

void pa_update_sinks();

pa_info_list* pa_get_default_sink();

static void pa_sink_change_cb(pa_context *c, 
        int success, void *userdata);

void pa_toggle_mute_default_sink();

int pa_cvolume_to_int(pa_cvolume* volume);
static void pa_change_pa_cvolume(pa_cvolume* volume, int delta);

void pa_change_volume_default_sink(int delta);
void pa_new_default_sink(pa_info_list* sink, bool notify);

void pa_cycle_sink();

pa_info_list* pa_get_sinks();

void pa_get_icon_name(string icon);

void pa_volume_text(char* text);
#endif
