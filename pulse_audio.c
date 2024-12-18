#include "pulse_audio.h"
#include "glib.h"
#include "status_icon.h"
#include <stdlib.h>

static pa_mainloop *mainloop = NULL;
static pa_context *context = NULL;
static pa_mainloop_api *mainloop_api = NULL;
static pa_context_state_t pa_state = PA_CONTEXT_CONNECTING;

void pa_init() {
    mainloop = pa_mainloop_new();
    mainloop_api = pa_mainloop_get_api(mainloop);
    context = pa_context_new(mainloop_api, "XApp");

    pa_context_connect(context, NULL, PA_CONTEXT_NOFLAGS, NULL);
    pa_context_set_state_callback(context, pa_state_cb, NULL);
}

void pa_run() {
    pa_mainloop_run(mainloop, NULL);
}

void pa_quit() {
    pa_context_disconnect(context);
    pa_context_unref(context);
    pa_mainloop_free(mainloop);
}

void pa_state_cb(pa_context* c, void* userdata) {
    pa_state = pa_context_get_state(c);
}

gboolean pa_step() {
    pa_mainloop_iterate(mainloop, 0, NULL);
    return TRUE;
}

void pa_wait_for_ready() {
    while (pa_state != PA_CONTEXT_READY) {
        pa_step();
    }
}

void pa_wait_for_operation(pa_operation* op) {
    while (pa_operation_get_state(op) != PA_OPERATION_DONE) {
        pa_step();
    }
    pa_operation_unref(op);
}

pa_context* pa_get_context() {
    return context;
}

void pa_subscribe_cb(pa_context *c, pa_subscription_event_type_t t, 
        uint32_t idx, void *userdata) {
    if (t & PA_SUBSCRIPTION_EVENT_CHANGE) {
        g_idle_add((GSourceFunc)update_icon, NULL);
    }
}

void pa_subscribe_to_sink_changes(gpointer app) {
    pa_wait_for_ready();
    pa_context* context = pa_get_context();

    pa_context_set_subscribe_callback(context, pa_subscribe_cb, NULL);
    pa_operation* pa_op = pa_context_subscribe(context, PA_SUBSCRIPTION_MASK_SINK, NULL, NULL);
    pa_wait_for_operation(pa_op);

    g_timeout_add(20, (GSourceFunc)pa_step, app);
}
