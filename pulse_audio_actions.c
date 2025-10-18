#include "pulse_audio_actions.h"
#include <pulse/volume.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "pulse_audio.h"
#include "notify.h"
#include "status_icon.h"

pa_info_list* sinks = NULL;

pa_info_list* pa_new_info_list(const char* name,
        const char* id, bool is_default,
        pa_cvolume volume, bool mute) {
    pa_info_list* new_info = malloc(sizeof(pa_info_list));
    strcpy(new_info->name, name);
    strcpy(new_info->id, id);

    new_info->is_default = is_default;
    new_info->next = NULL;

    new_info->volume = volume;
    new_info->mute = mute;
    return new_info;
}

void open_pavucontrol() {
    pid_t pid = fork();
    if (pid == 0) {
        execlp("pavucontrol", "pavucontrol", NULL);
        // If execlp fails
        g_error("Failed to start pavucontrol: %s\n", g_strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Fork failed
        g_error("Fork failed: %s\n", g_strerror(errno));
    }
}

void pa_free_sinks() {
    pa_info_list* current = sinks;
    while (current != NULL) {
        pa_info_list* next = current->next;
        free(current);
        current = next;
    }
    sinks = NULL;
}

pa_info_list* pa_get_sink_by_id(const char* id) {
    for (pa_info_list* sink = sinks; sink != NULL; sink = sink->next) {
        if (strcmp(sink->id, id) == 0)
            return sink;
    }
    return NULL;
}

void pa_update_sinks_sinklist_cb(pa_context *c, const pa_sink_info *l, 
        int eol, void *userdata) {
    if (l == NULL || eol) {
        update_icon();
        return;
    }

    if (pa_get_sink_by_id(l->name) != NULL)
        return;

    pa_info_list* new_sink = pa_new_info_list(l->description, l->name,
            strcmp(l->name, userdata) == 0, l->volume, l->mute);
    new_sink->next = sinks;
    sinks = new_sink;
}

void pa_update_sinks_get_default_sink_cb(pa_context *c, 
        const pa_server_info *i, void *userdata) {
    char* default_sink_id = malloc(strlen(i->default_sink_name) + 1);
    strcpy(default_sink_id, i->default_sink_name);

    pa_operation* op = pa_context_get_sink_info_list(c, 
            pa_update_sinks_sinklist_cb, default_sink_id);
    pa_operation_unref(op);
}

void pa_update_sinks() {
    pa_free_sinks();
    sinks = NULL;

    pa_operation* op = pa_context_get_server_info(pa_get_context(),
            pa_update_sinks_get_default_sink_cb, NULL);
    pa_operation_unref(op);
}

pa_info_list* pa_get_default_sink() {
    for (pa_info_list* sink = sinks; sink != NULL; sink = sink->next) {
        if (sink->is_default)
            return sink;
    }
    return NULL;
}

void pa_sink_change_cb(pa_context *c, int success, void *userdata) {
    pa_info_list* sink = (pa_info_list*) userdata;

    if (success) {
        notify_sink_change(sink);
    }
}

void pa_toggle_mute_default_sink() {
    pa_info_list* default_sink = pa_get_default_sink();
    if (default_sink == NULL)
        return;

    default_sink->mute = !default_sink->mute;

    pa_operation* op = pa_context_set_sink_mute_by_name(pa_get_context(),
            default_sink->id, default_sink->mute, 
            pa_sink_change_cb, default_sink);
    pa_operation_unref(op);
}

int pa_cvolume_to_int(pa_cvolume* volume) {
    return roundf(100.0f * pa_cvolume_avg(volume) / PA_VOLUME_NORM);
}

void pa_change_pa_cvolume(pa_cvolume* volume, int delta) {
    int percentage = pa_cvolume_to_int(volume);

    percentage += delta;
    percentage = MAX(0, MIN(100, percentage));

    pa_volume_t new_volume = percentage * PA_VOLUME_NORM / 100;
    pa_cvolume_set(volume, volume->channels, new_volume);
}

void pa_change_volume_unmute_cb(pa_context *c, 
        int success, void *userdata) {
    pa_info_list* sink = (pa_info_list*) userdata;

    pa_operation* op = pa_context_set_sink_volume_by_name(
            pa_get_context(), sink->id, 
            &sink->volume, pa_sink_change_cb, sink);
    pa_operation_unref(op);
}

void pa_change_volume_default_sink(int delta) {
    pa_info_list* default_sink = pa_get_default_sink();
    if (default_sink == NULL)
        return;

    pa_change_pa_cvolume(&default_sink->volume, delta);
    default_sink->mute = false;

    pa_operation* op = pa_context_set_sink_mute_by_name(
            pa_get_context(), default_sink->id, 
            false, pa_change_volume_unmute_cb, default_sink);
    pa_operation_unref(op);

}

void pa_new_default_sink_cb(pa_context *c, int success, void *userdata) {
    pa_info_list* new_default = (pa_info_list*) userdata;

    if (success && new_default)
        notify_new_default_sink(new_default);
}

void pa_new_default_sink(pa_info_list* new_default,
        bool notify) {
    pa_info_list* old_default = pa_get_default_sink();
    if (old_default != NULL)
        old_default->is_default = false;

    new_default->is_default = true;

    pa_operation* op = pa_context_set_default_sink(pa_get_context(),
            new_default->id, pa_new_default_sink_cb, 
            notify ? new_default : NULL);
    pa_operation_unref(op);
}

void pa_cycle_sink() {
    pa_info_list* it = sinks;

    if (it == NULL)
        return;

    while (!it->is_default) {
        it = it->next;
    }

    pa_info_list* new_default = it->next;
    if (new_default == NULL)
        new_default = sinks;

    pa_new_default_sink(new_default, true);
}

pa_info_list* pa_get_sinks() {
    return sinks;
}

void pa_get_icon_name(string icon) {
    const char* base = get_icon_path();

    pa_info_list* default_sink = pa_get_default_sink();
    if (default_sink == NULL) {
        snprintf(icon, MAX_STR_LEN, "%s%s", base, "muted.svg");
    } else {
        int volume = pa_cvolume_to_int(&default_sink->volume);

        if (default_sink->mute || volume == 0) {
            snprintf(icon, MAX_STR_LEN, "%s%s", base, "muted.svg");
        } else if (volume < 33) {
            snprintf(icon, MAX_STR_LEN, "%s%s", base, "low.svg");
        } else if (volume < 66) {
            snprintf(icon, MAX_STR_LEN, "%s%s", base, "medium.svg");
        } else {
            snprintf(icon, MAX_STR_LEN, "%s%s", base, "high.svg");
        }
    }
}

void pa_volume_text(char* text) {
    pa_info_list* default_sink = pa_get_default_sink();
    if (default_sink == NULL) {
        strcpy(text, "N/A");
        return; 
    }

    int volume = pa_cvolume_to_int(&default_sink->volume);
    bool mute = default_sink->mute;

    sprintf(text, "%d%%%s", volume, mute ? " [muted]" : "");
}
