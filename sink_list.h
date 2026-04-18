#ifndef SINK_LIST_H
#define SINK_LIST_H

#include <glib.h>
#include <stdint.h>
#include <pulse/pulseaudio.h>

typedef struct {
    uint32_t index;
    GString* name;
    GString* id;
    gboolean is_muted;
    pa_cvolume volume;
    int last_notified_volume_percent;
    struct timespec created_at;
} SinkInfo;

SinkInfo* sink_list_add_or_update(const char* name, const char* id, uint32_t index,
    gboolean is_muted, const pa_cvolume* volume, gboolean* notify, gboolean* update_icon);
void sink_list_clear();
void sink_list_remove_by_index(uint32_t index);
SinkInfo* sink_list_get_by_index(uint32_t index);
gboolean sink_list_update_default(const char* default_id);
gboolean sink_info_is_default(SinkInfo* info);
gboolean sink_list_has_default();
SinkInfo* sink_list_get_default();
SinkInfo* sink_list_get_default_next();
GList* sink_list_get_all();

#endif
