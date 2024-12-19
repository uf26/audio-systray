#include "pulse_audio.h"
#include "glib.h"
#include <stdlib.h>
#include <pulse/glib-mainloop.h>

static pa_glib_mainloop *mainloop = NULL;
static pa_mainloop_api *mainloop_api = NULL;
static pa_context *context = NULL;
static pa_context_state_t pa_state = PA_CONTEXT_CONNECTING;

void pa_init() {
    mainloop = pa_glib_mainloop_new(NULL);
    mainloop_api = pa_glib_mainloop_get_api(mainloop);
    context = pa_context_new(mainloop_api, "audio systray");

    pa_context_connect(context, NULL, PA_CONTEXT_NOFLAGS, NULL);
    pa_context_set_state_callback(context, pa_ready_cb, NULL);
}

void pa_quit() {
    pa_context_disconnect(context);
    pa_context_unref(context);
    pa_glib_mainloop_free(mainloop);
}

pa_context* get_context() {
    return context;
}

void pa_ready_cb(pa_context* c, void* userdata) {
    pa_context_state_t state = pa_context_get_state(c);
    if (state == PA_CONTEXT_READY) {
        pa_subscribe();
    }
}

void pa_subscribe_cb(pa_context *c, pa_subscription_event_type_t t, 
        uint32_t idx, void *userdata) {
    if (t & PA_SUBSCRIPTION_EVENT_CHANGE) {
        g_print("[SUBSCRIBE] Sink changed (index: %u)\n", idx);
    }
}

void pa_subscribe() {
    pa_context_set_subscribe_callback(context, pa_subscribe_cb, NULL);

    pa_operation *o = pa_context_subscribe(context, 
            PA_SUBSCRIPTION_MASK_SINK, NULL, NULL);
    pa_operation_unref(o);
}

