#ifndef PULSE_AUDIO_ACTIONS_H
#define PULSE_AUDIO_ACTIONS_H

#include <pulse/pulseaudio.h>
#include <string>

struct pa_info_list {
    std::string name;
    std::string id;
    bool is_default;
    pa_info_list *next;

    pa_info_list() : next(NULL) {}

    pa_info_list(std::string name, std::string id, bool is_default) 
        : name(name), id(id), is_default(is_default), next(NULL) {}

    static void free_list(pa_info_list *list) {
        while (list != NULL) {
            pa_info_list *next = list->next;
            delete list;
            list = next;
        }
    }
};

void open_pavucontrol();

void pa_get_default_sink_id_cb(pa_context *c, const pa_server_info *i, 
        void *userdata);
std::string pa_get_default_sink_id();
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
void pa_set_volume(int volume);

#endif
