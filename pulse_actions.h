#ifndef PULSE_AUDIO_ACTIONS_H
#define PULSE_AUDIO_ACTIONS_H

#include <stdbool.h>

void pa_change_volume(int step);
void pa_toggle_mute();
void pa_cycle_sink(bool notify);
void pa_open_pavucontrol();

#endif
