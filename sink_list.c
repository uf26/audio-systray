#include "sink_list.h"
#include <time.h>
#include "utils.h"

#define SINK_INFO_INITAL_SLEEP_MS 500

static GList* sink_list = NULL;
static GString* default_sink_id = NULL;

void sink_info_free(SinkInfo *info) {
    if (!info) return;
    g_string_free(info->name, TRUE);
    g_string_free(info->id, TRUE);
    g_free(info);
}

SinkInfo* sink_list_add(const char* name, const char* id, uint32_t index,
    gboolean is_muted, const pa_cvolume* volume) {
    SinkInfo* info = g_new0(SinkInfo, 1);
    info->name = g_string_new(name);
    info->id = g_string_new(id);
    info->index = index;
    info->is_muted = is_muted;
    info->volume = volume ? *volume : (pa_cvolume){0};
    clock_gettime(CLOCK_MONOTONIC, &info->created_at);

    sink_list = g_list_append(sink_list, info);

    return info;
}

SinkInfo* sink_list_add_or_update(const char* name, const char* id, uint32_t index,
        gboolean is_muted, const pa_cvolume* volume, gboolean* notify, gboolean* update_icon) {
    *notify = FALSE;
    *update_icon = FALSE;
    
    SinkInfo* info = sink_list_get_by_index(index);
    if (!info) {
        info = sink_list_add(name, id, index, is_muted, volume);
        *update_icon = sink_info_is_default(info);
        return info;
    }

    gboolean changed = FALSE;
    if (info->is_muted != is_muted) {
        info->is_muted = is_muted;
        changed = TRUE;
    }
    if (volume && !pa_cvolume_equal(&info->volume, volume)) {
        pa_volume_t old_avg = pa_cvolume_avg(&info->volume);
        pa_volume_t new_avg = pa_cvolume_avg(volume);
        pa_volume_t five_percent = (PA_VOLUME_NORM * 5) / 100;
        if (old_avg == 0 && new_avg > five_percent) {
            changed = TRUE;
        }

        info->volume = *volume;
    }

    if (!sink_info_is_default(info) || !changed)
        return NULL;

    *notify = check_timeout(info->created_at, SINK_INFO_INITAL_SLEEP_MS);
    *update_icon = TRUE;

    return info;
}

void sink_list_clear() {
    if (!sink_list) return;

    g_list_free_full(sink_list, (GDestroyNotify)sink_info_free);
    sink_list = NULL;

    if (default_sink_id) {
        g_string_free(default_sink_id, TRUE);
        default_sink_id = NULL;
    }
}

void sink_list_remove_by_index(uint32_t index) {
    GList* iter = sink_list;
    while (iter) {
        SinkInfo* info = (SinkInfo*)iter->data;
        
        if (info->index == index) {
            sink_info_free(info);
            sink_list = g_list_delete_link(sink_list, iter);
            return;
        }
        iter = iter->next;
    }
}

SinkInfo* sink_list_get_by_index(uint32_t index) {
    GList* iter = sink_list;
    while (iter) {
        SinkInfo* info = (SinkInfo*)iter->data;
        if (info->index == index) {
            return info;
        }
        iter = iter->next;
    }
    return NULL;
}

gboolean sink_list_update_default(const char* default_id) {
    if (!default_sink_id) {
        default_sink_id = g_string_new(default_id);
        return FALSE;
    } else {
        gboolean changed = strcmp(default_sink_id->str, default_id) != 0;
        g_string_assign(default_sink_id, default_id);
        return changed;
    }
}

gboolean sink_info_is_default(SinkInfo* info) {
    if (!default_sink_id) return FALSE;
    return strcmp(info->id->str, default_sink_id->str) == 0;
}

SinkInfo* sink_list_get_default() {
    if (!default_sink_id) return NULL;

    GList* iter = sink_list;
    while (iter) {
        SinkInfo* info = (SinkInfo*)iter->data;
        if (strcmp(info->id->str, default_sink_id->str) == 0) {
            return info;
        }
        iter = iter->next;
    }
    return NULL;
}

SinkInfo* sink_list_get_default_next() {
    if (!default_sink_id) return NULL;

    GList* iter = sink_list;
    while (iter) {
        SinkInfo* info = (SinkInfo*)iter->data;
        if (strcmp(info->id->str, default_sink_id->str) == 0) {
            if (iter->next) {
                return (SinkInfo*)iter->next->data;
            } else {
                return (SinkInfo*)sink_list->data; 
            }
        }
        iter = iter->next;
    }
    return NULL;
}

gboolean sink_list_has_default() {
    return default_sink_id != NULL;
}

GList* sink_list_get_all() {
    return sink_list;
}
