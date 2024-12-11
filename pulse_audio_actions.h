#ifndef PULSE_AUDIO_ACTIONS_H
#define PULSE_AUDIO_ACTIONS_H

#include <pulse/pulseaudio.h>
#include <list>

void open_pavucontrol();

static void pa_sinklist_cb(pa_context *c, const pa_sink_info *l, 
        int eol, void *userdata);
std::list<pa_sink_info*> pa_get_sinks();

static void pa_source_cb(pa_context *c, const pa_source_info *l, 
        int eol, void *userdata);
std::list<pa_source_info*> pa_get_sources();

#endif
