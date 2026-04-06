#include "utils.h"
#include <time.h>

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

gboolean check_timeout(struct timespec start, int timeout_ms) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    long elapsed_ms = (now.tv_sec - start.tv_sec) * 1000 + 
        (now.tv_nsec - start.tv_nsec) / 1000000;

    if (elapsed_ms < timeout_ms) {
        return FALSE; 
    }

    return TRUE;
}

gchar* truncate_utf8_string(const gchar* text, glong max_len) {
    if (text == NULL) return NULL;

    glong actual_len = g_utf8_strlen(text, -1);

    if (actual_len <= max_len) {
        return g_strdup(text);
    }

    glong truncate_at = (max_len > 3) ? (max_len - 3) : 0;

    gchar *truncated = g_utf8_substring(text, 0, truncate_at);
    gchar *result = g_strdup_printf("%s...", truncated);
    
    g_free(truncated);
    return result;
}
