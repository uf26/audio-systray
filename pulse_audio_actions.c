#include "pulse_audio_actions.h"
#include <pulse/introspect.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "pulse_audio.h"
#include "notify.h"

pa_info_list* new_pa_info_list(const char* name, 
        const char* id, bool is_default, pa_info_list* next) {
    pa_info_list* info_list = malloc(sizeof(pa_info_list));
    strcpy(info_list->name, name);
    strcpy(info_list->id, id);
    info_list->is_default = is_default;
    info_list->next = next;

    return info_list;
}

pa_sink_volume* new_pa_sink_volume(int volume, bool mute, 
        pa_cvolume* cvolume) {
    pa_sink_volume* sink_volume = malloc(sizeof(pa_sink_volume));
    sink_volume->volume = volume;
    sink_volume->mute = mute;
    sink_volume->cvolume = cvolume;

    return sink_volume;
}

void open_pavucontrol() {
    if (fork() == 0) {
        execlp("pavucontrol", "pavucontrol", NULL);
    }
}

void pa_sink_changed_cb(pa_context *c, int eol, void *userdata) {
    pa_sink_volume* sink = (pa_sink_volume*) userdata;

    if(!sink)
        return;

    notify_send_volume(sink->volume, sink->mute);

    if (sink->cvolume)
        free(sink->cvolume);
    free(sink);
}

void pa_toggle_mute_cb(pa_context* c, const pa_sink_info* s, int eol,
        void *userdata) {
    if(s == NULL || eol)
        return;

    bool mute = !s->mute;

    int volume = pa_cvolume_to_int(&s->volume);
    pa_sink_volume* sink_volume = new_pa_sink_volume(
            volume, mute, NULL);

    pa_operation *pa_op = pa_context_set_sink_mute_by_index(
            get_context(), s->index, 
            mute, pa_sink_changed_cb, sink_volume);
    pa_operation_unref(pa_op);
}

void pa_toggle_mute_default_sink_cb(pa_context *c,
        const pa_server_info *i, void *userdata) {
    if(i == NULL)
        return;

    pa_operation *pa_op = pa_context_get_sink_info_by_name(get_context(), 
            i->default_sink_name, pa_toggle_mute_cb, NULL);
    pa_operation_unref(pa_op);
}

void pa_toggle_mute_default_sink() {
    pa_context* context = get_context();

    pa_operation *pa_op = pa_context_get_server_info(context, 
            pa_toggle_mute_default_sink_cb, NULL);
    pa_operation_unref(pa_op);
}


int pa_cvolume_to_int(const pa_cvolume* volume) {
    return roundf(100.0f * pa_cvolume_avg(volume) / PA_VOLUME_NORM);
}

pa_cvolume* int_to_pa_cvolume(int volume, int nr_channels) {
    pa_cvolume* cvolume = malloc(sizeof(pa_cvolume));
    pa_cvolume_set(cvolume, nr_channels, volume * PA_VOLUME_NORM / 100);

    return cvolume;
}

void pa_change_volume_cb(pa_context *c, const pa_sink_info* s, int eol, 
        void *userdata) {
    if(s == NULL || eol)
        return;

    int delta = (int)(long unsigned)userdata;

    int old_volume = pa_cvolume_to_int(&s->volume);
    int new_volume = old_volume + delta;
    new_volume = MAX(MIN(new_volume, 100), 0);

    pa_sink_volume* sink_volume = new_pa_sink_volume(
            new_volume, false, 
            int_to_pa_cvolume(new_volume, s->volume.channels));

    pa_operation *o_mute = pa_context_set_sink_mute_by_name(
            c, s->name, 0, NULL, NULL);
    pa_operation_unref(o_mute);

    pa_operation *o_vol = pa_context_set_sink_volume_by_name(
            c, s->name, sink_volume->cvolume, 
            pa_sink_changed_cb, sink_volume);
    pa_operation_unref(o_vol);
}

void pa_change_volume_default_sink_cb(pa_context *c, 
        const pa_server_info *i, void *userdata) {
    if(i == NULL)
        return;

    int delta = (int)(long unsigned)userdata;
    pa_operation *pa_op = pa_context_get_sink_info_by_name(get_context(), 
            i->default_sink_name, pa_change_volume_cb, 
            (void*)(long unsigned)delta);
    pa_operation_unref(pa_op);
}

void pa_change_volume_default_sink(int delta) {
    pa_context* context = get_context();

    pa_operation *pa_op = pa_context_get_server_info(context, 
            pa_change_volume_default_sink_cb, 
            (void*)(long unsigned)delta);
    pa_operation_unref(pa_op);
}


void pa_new_default_sink_cb(pa_context *c, int eol, void *userdata) {
    pa_info_list* sink = (pa_info_list*) userdata;

    notify_send_cycle_sink(sink->name);
    free(sink);
}

void pa_set_default_sink(pa_info_list* sink) {
    pa_context* context = get_context();

    pa_operation *pa_op = pa_context_set_default_sink(context, 
            sink->id, pa_new_default_sink_cb, sink);
    pa_operation_unref(pa_op);
}


void pa_cycle_sink_cb(pa_context *c, const pa_sink_info* i, 
        int eol, void *userdata) {
    pa_info_list* default_sink = (pa_info_list*) userdata;

    if (strlen(default_sink->id) == 0)
        return;

    if (i == NULL || eol) {
        if (default_sink->next)
            pa_set_default_sink(default_sink->next);
        free(default_sink);
        return;
    }

    if (strcmp(i->name, default_sink->id) == 0) {
        default_sink->is_default = true;
    } else if (default_sink->is_default) {
        pa_info_list* new_sink = new_pa_info_list(
            i->description, i->name, false, NULL);
        pa_set_default_sink(new_sink);
        memset(default_sink->id, 0, MAX_STR_LEN);

        free(default_sink);
        free(default_sink->next);
    } else if (default_sink->next == NULL) {
        default_sink->next = new_pa_info_list(
                i->description, i->name, false, NULL);
    }
}

void pa_cycle_sink_get_default_cb(pa_context *c, 
        const pa_server_info* s, void *userdata) {
    if(s == NULL)
        return;

    pa_info_list* default_sink = new_pa_info_list("", 
            s->default_sink_name, false, NULL);

    pa_operation* o = pa_context_get_sink_info_list(c,
            pa_cycle_sink_cb, default_sink);
}

void pa_cycle_sink() {
    pa_context* context = get_context();

    pa_operation *pa_op = pa_context_get_server_info(context, 
            pa_cycle_sink_get_default_cb, NULL);
    pa_operation_unref(pa_op);
}


void pa_menu_get_sinks_cb(pa_context *c, const pa_sink_info *i, 
        int eol, void *userdata) {
    pa_info_list* default_sink = (pa_info_list*) userdata;

    if (i == NULL || eol) {
        pa_info_list* it = default_sink->next;

        while(it) {
            g_print("Sink: %s, default: %d\n", 
                    it->name, it->is_default);
            it = it->next;
        }
        return;
    }

    pa_info_list* sink = new_pa_info_list(i->description, 
            i->name, 
            strcmp(i->name, default_sink->id) == 0,
            default_sink->next);

    default_sink->next = sink;
}

void pa_menu_get_default_sink_cb(pa_context *c, 
        const pa_server_info *i, void *userdata) {
    if(i == NULL)
        return;

    pa_info_list* sinks = new_pa_info_list("", 
            i->default_sink_name, true, NULL);
    pa_operation* o = pa_context_get_sink_info_list(c, 
            pa_menu_get_sinks_cb, sinks);
    pa_operation_unref(o);
}

void pa_create_menu() {
    pa_context* context = get_context();

    pa_operation* o = pa_context_get_server_info(context, 
            pa_menu_get_default_sink_cb, NULL);
    pa_operation_unref(o);
}
