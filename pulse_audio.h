#ifndef PULSE_AUDIO_H
#define PULSE_AUDIO_H

#include <pulse/pulseaudio.h>

void pa_init();
void pa_run();
void pa_quit();

static void pa_state_cb(pa_context* c, void* userdata);
static void pa_step();
void pa_wait_for_ready();
void pa_wait_for_operation(pa_operation* op);
pa_context* pa_get_context();

#endif
