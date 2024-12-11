#ifndef PULSE_AUDIO_ACTIONS_H
#define PULSE_AUDIO_ACTIONS_H

#include <pulse/pulseaudio.h>

void open_pavucontrol();

static void pa_sinklist_cb(pa_context *c, const pa_sink_info *l, 
        int eol, void *userdata);
void pa_print_sinks();

static void pa_source_cb(pa_context *c, const pa_source_info *l, 
        int eol, void *userdata);
void pa_print_sources();

#endif
