#ifndef PULSE_AUDIO_ACTIONS_H
#define PULSE_AUDIO_ACTIONS_H

#include <pulse/pulseaudio.h>
#include <stdbool.h>

#define MAX_STR_LEN 100

typedef char string[MAX_STR_LEN];

typedef struct pa_info_list_t {
    string name;
    string id;
    bool is_default;
    struct pa_info_list_t *next;
}pa_info_list;

static pa_info_list* new_pa_info_list(const char* name,
        const char* id, bool is_default, pa_info_list* next);

typedef struct {
    int volume;
    bool mute;
    pa_cvolume* cvolume;
} pa_sink_volume;

static pa_sink_volume* new_pa_sink_volume(int volume, bool mute,
        pa_cvolume* cvolume);

void open_pavucontrol();

static void pa_sink_changed_cb(pa_context *c, int eol, void *userdata);


static void pa_toggle_mute_cb(pa_context* c, const pa_sink_info* s, 
        int eol, void *userdata);
static void pa_toggle_mute_default_sink_cb(pa_context *c,
        const pa_server_info *i, void *userdata);
void pa_toggle_mute_default_sink();


static int pa_cvolume_to_int(const pa_cvolume* volume);
static pa_cvolume* int_to_pa_cvolume(int volume, int nr_channels);
static void pa_change_volume_cb(pa_context *c, const pa_sink_info* s, 
        int eol, void *userdata);
static void pa_change_volume_default_sink_cb(pa_context *c, 
        const pa_server_info *i, void *userdata);
void pa_change_volume_default_sink(int delta);


void pa_set_default_sink(pa_info_list* sink);


static void pa_cycle_sink_cb(pa_context *c, const pa_sink_info* i, 
        int eol, void *userdata);
static void pa_cycle_sink_get_default_cb(pa_context *c, 
        const pa_server_info* s, void *userdata);
void pa_cycle_sink();


static void pa_menu_get_sinks_cb(pa_context *c, const pa_sink_info *i, 
        int eol, void *userdata);
void pa_menu_get_default_sink_cb(pa_context *c, 
        const pa_server_info *i, void *userdata);
void pa_create_menu();

#endif
