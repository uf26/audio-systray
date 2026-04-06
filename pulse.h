#ifndef PULSE_AUDIO_H
#define PULSE_AUDIO_H

#include <glib.h>
#include <pulse/pulseaudio.h>

void pa_init();
void pa_quit();

void pa_update_sink(uint32_t index, const pa_cvolume* volume, gboolean is_muted);
void pa_set_default_sink(const char* id);

#endif
