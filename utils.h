#ifndef UTILS_H
#define UTILS_H

#include <glib.h>

const gchar* get_volume_icon_name(int volume_percent, gboolean is_muted);
void free_icon_cache();

#endif
