#include "pulse_audio_actions.h"
#include <iostream>
#include <cmath>

#include "pulse_audio.h"

void open_pavucontrol() {
    if (fork() == 0) {
        execlp("pavucontrol", "pavucontrol", NULL);
    }
}

void pa_get_default_sink_id_cb(pa_context *c, const pa_server_info *i, 
        void *userdata) {
    if(i == NULL)
        return;

    std::string *default_sink_id = (std::string *) userdata;
    *default_sink_id = i->default_sink_name;
}

std::string pa_get_default_sink_id() {
    pa_wait_for_ready();

    pa_context* context = pa_get_context();
    std::string default_sink_id;
    pa_operation *pa_op = pa_context_get_server_info(context, 
            pa_get_default_sink_id_cb, &default_sink_id);

    pa_wait_for_operation(pa_op);

    return default_sink_id;
}

void pa_set_default_sink(pa_info_list* sink) {
    pa_wait_for_ready();

    pa_context* context = pa_get_context();
    pa_operation *pa_op = pa_context_set_default_sink(context, 
            sink->id.c_str(), NULL, NULL);
    pa_wait_for_operation(pa_op);
}

void pa_sinklist_cb(pa_context *c, const pa_sink_info *l, 
        int eol, void *userdata) {
    if (eol != 0 || l == NULL)
        return;

    pa_info_list** sinks = (pa_info_list**) userdata;
    pa_info_list* new_sink = new pa_info_list(
            l->description, l->name, false);

    if (*sinks == NULL)
        *sinks = new_sink;
    else {
        new_sink->next = *sinks;
        *sinks = new_sink;
    }
}

pa_info_list* pa_get_sinks() { 
    pa_info_list *sinks = NULL;

    pa_wait_for_ready();
    pa_context* context = pa_get_context();
    pa_operation *pa_op = pa_context_get_sink_info_list(
            context, pa_sinklist_cb, &sinks);
    pa_wait_for_operation(pa_op);

    std::string default_sink_id = pa_get_default_sink_id();
    for (pa_info_list* sink = sinks; sink != NULL; sink = sink->next) {
        if (sink->id == default_sink_id)
            sink->is_default = true;
    }
    return sinks;
}

void pa_cycle_sink() {
    std::string default_sink_id = pa_get_default_sink_id();
    pa_info_list* sinks = pa_get_sinks();

    pa_info_list* new_sink = sinks;
    while (new_sink->id != default_sink_id)
        new_sink = new_sink->next;

    pa_set_default_sink(new_sink);
}

int pa_cvolume_to_int(pa_cvolume* volume) {
    return roundf(100.0f * pa_cvolume_avg(volume) / PA_VOLUME_NORM);
}

pa_cvolume int_to_pa_cvolume(int volume, int nr_channels) {
    pa_cvolume cvolume;
    pa_cvolume_set(&cvolume, nr_channels, volume * PA_VOLUME_NORM / 100);
    return cvolume;
}

void pa_get_sink_info_cb(pa_context *c, 
        const pa_sink_info *i, int eol, void *userdata) {
    if (eol != 0 || i == NULL)
        return;

    pa_sink_info* sink_info = (pa_sink_info*) userdata;
    *sink_info = *i;
}

pa_sink_info pa_get_sink_info() {
    std::string default_sink_id = pa_get_default_sink_id();

    pa_wait_for_ready();

    pa_context* context = pa_get_context();
    pa_sink_info sink_info;
    pa_operation* pa_op = pa_context_get_sink_info_by_name(context, 
            default_sink_id.c_str(), pa_get_sink_info_cb, &sink_info);
    pa_wait_for_operation(pa_op);

    return sink_info;
}

void pa_get_volume(int* volume, bool* mute) {
    pa_sink_info sink_info = pa_get_sink_info();

    *volume = pa_cvolume_to_int(&sink_info.volume);
    *mute = sink_info.mute;
}

void pa_toggle_mute() {
    bool mute;
    int volume;
    pa_get_volume(&volume, &mute);
    pa_set_mute(!mute);
}

void pa_set_mute(bool mute) {
    std::string default_sink_id = pa_get_default_sink_id();

    pa_wait_for_ready();
    pa_context* context = pa_get_context();

    pa_operation* pa_op = pa_context_set_sink_mute_by_name(context, 
            default_sink_id.c_str(), mute, NULL, NULL);
    pa_wait_for_operation(pa_op);
}

void pa_set_volume(int delta) {
    pa_sink_info sink_info = pa_get_sink_info();
    int new_volume = pa_cvolume_to_int(&sink_info.volume) + delta;
    new_volume = std::max(0, std::min(100, new_volume));
    pa_cvolume new_cvolume = int_to_pa_cvolume(
            new_volume, sink_info.volume.channels);

    std::string default_sink_id = pa_get_default_sink_id();

    pa_set_mute(false);

    pa_wait_for_ready();
    pa_context* context = pa_get_context();

    pa_operation* pa_op = pa_context_set_sink_volume_by_name(context, 
            default_sink_id.c_str(), &new_cvolume, NULL, NULL);
    pa_wait_for_operation(pa_op);
}

