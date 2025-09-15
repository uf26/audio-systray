#include "pulse_audio.h"
#include <glib.h>
#include <stdlib.h>
#include <threads.h>
#include <pulse/glib-mainloop.h>
#include "pulse_audio_actions.h"

#define SUBSCRIPTION_MASK (PA_SUBSCRIPTION_MASK_SINK\
        | PA_SUBSCRIPTION_MASK_SERVER)
#define EVENT_MASK (PA_SUBSCRIPTION_EVENT_CHANGE\
        | PA_SUBSCRIPTION_EVENT_NEW | PA_SUBSCRIPTION_EVENT_REMOVE)

static pa_glib_mainloop* mainloop = NULL;
static pa_context *context = NULL;
static pa_mainloop_api *mainloop_api = NULL;
static size_t pa_restart_attempts = 0;

void pa_restart() {
    pa_context_disconnect(context);
    pa_context_unref(context);
    pa_free_sinks();

    context = pa_context_new(mainloop_api, "audio_systray");
    pa_context_connect(context, NULL, PA_CONTEXT_NOFLAGS, NULL);
    pa_context_set_state_callback(context, pa_state_cb, NULL);
}

void pa_init() {
    mainloop = pa_glib_mainloop_new(NULL);
    mainloop_api = pa_glib_mainloop_get_api(mainloop);
    context = pa_context_new(mainloop_api, "audio_systray");

    pa_context_connect(context, NULL, PA_CONTEXT_NOFLAGS, NULL);
    pa_context_set_state_callback(context, pa_state_cb, NULL);
}

void pa_quit() {
    pa_context_disconnect(context);
    pa_context_unref(context);
    pa_glib_mainloop_free(mainloop);
    pa_free_sinks();
}

void pa_state_cb(pa_context* c, void* userdata) {
    switch (pa_context_get_state(c)) {
        case PA_CONTEXT_READY:
            pa_restart_attempts = 0;

            pa_update_sinks();
            pa_subscribe_to_sink_changes();
            break;

        case PA_CONTEXT_FAILED:
        case PA_CONTEXT_TERMINATED:
            if (pa_restart_attempts++ > 10) {
                g_printerr("Failed to connect to PulseAudio after 10 attempts, quitting.\n");
                sleep(30);
                exit(1);
            }

            g_idle_add((GSourceFunc)pa_restart, NULL);
            break;

        default:;
    };
}

pa_context* pa_get_context() {
    return context;
}

void pa_subscribe_cb(pa_context *c, pa_subscription_event_type_t t, 
        uint32_t idx, void *userdata) {
    if (t & EVENT_MASK) {
        pa_update_sinks();
    }
}

void pa_subscribe_to_sink_changes() {
    pa_context_set_subscribe_callback(context, pa_subscribe_cb, NULL);
    pa_operation* pa_op = pa_context_subscribe(context, 
            SUBSCRIPTION_MASK, NULL, NULL);
    pa_operation_unref(pa_op);
}
