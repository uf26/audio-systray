#ifndef PULSE_AUDIO_H
#define PULSE_AUDIO_H

#include <pulse/pulseaudio.h>
#include <gtk/gtk.h>

void pa_init();
void pa_quit();

static void pa_state_cb(pa_context* c, void* userdata);
pa_context* pa_get_context();

static void pa_subscribe_cb(pa_context *c, 
        pa_subscription_event_type_t t, 
        uint32_t idx, void *userdata);
static void pa_subscribe_to_sink_changes();

#endif
