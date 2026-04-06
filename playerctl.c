#include <playerctl/playerctl.h>

PlayerctlPlayer* get_best_player() {
    GError *error = NULL;
    PlayerctlPlayer *player = playerctl_player_new(NULL, &error);

    if (error) {
        g_debug("Playerctl: No player found: %s", error->message);
        g_error_free(error);
        return NULL;
    }
    return player;
}

void playerctl_next() {
    PlayerctlPlayer *player = get_best_player();
    if (!player) return;

    playerctl_player_next(player, NULL);
    g_object_unref(player);
}

void playerctl_previous() {
    PlayerctlPlayer *player = get_best_player();
    if (!player) return;

    playerctl_player_previous(player, NULL);
    g_object_unref(player);
}

void playerctl_play_pause() {
    PlayerctlPlayer *player = get_best_player();
    if (!player) return;

    playerctl_player_play_pause(player, NULL);
    g_object_unref(player);
}
