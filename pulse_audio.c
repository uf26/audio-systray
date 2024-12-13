#include "pulse_audio.h"
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

void pa_step() {
    pa_mainloop_iterate(mainloop, 0, NULL);
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
