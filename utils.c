#include "utils.h"

static gchar* icon_cache[4] = {NULL, NULL, NULL, NULL};

const gchar* get_volume_icon_name(int volume_percent, gboolean is_muted) {
    if (icon_cache[0] == NULL) {
        const gchar* base = g_getenv("ICON_PATH"); 
        
        if (base != NULL && g_file_test(base, G_FILE_TEST_IS_DIR)) {
            icon_cache[0] = g_build_filename(base, "muted.svg", NULL);
            icon_cache[1] = g_build_filename(base, "low.svg", NULL);
            icon_cache[2] = g_build_filename(base, "medium.svg", NULL);
            icon_cache[3] = g_build_filename(base, "high.svg", NULL);
        } else {
            icon_cache[0] = g_strdup("audio-volume-muted-symbolic");
            icon_cache[1] = g_strdup("audio-volume-low-symbolic");
            icon_cache[2] = g_strdup("audio-volume-medium-symbolic");
            icon_cache[3] = g_strdup("audio-volume-high-symbolic");
        }
    }

    if (is_muted || volume_percent == 0) {
        return icon_cache[0];
    } else if (volume_percent < 33) {
        return icon_cache[1];
    } else if (volume_percent < 66) {
        return icon_cache[2];
    } else {
        return icon_cache[3];
    }
}

void free_icon_cache() {
    for (int i = 0; i < 4; i++) {
        if (icon_cache[i]) {
            g_free(icon_cache[i]);
            icon_cache[i] = NULL;
        }
    }
}
