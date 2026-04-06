#ifndef UTILS_H
#define UTILS_H

#include <glib.h>

const gchar* get_volume_icon_name(int volume_percent, gboolean is_muted);
void free_icon_cache();

gboolean check_timeout(struct timespec start, int timeout_ms);

gchar* truncate_utf8_string(const gchar* text, glong max_len);

#endif
