#include "pulse_audio_actions.h"
#include <iostream>

#include "pulse_audio.h"

void open_pavucontrol() {
    system("pavucontrol");
}

void pa_sinklist_cb(pa_context *c, const pa_sink_info *l, 
        int eol, void *userdata) {
    if (eol != 0 || l == NULL)
        return;

    std::cout << "Sink: " << l->description << std::endl;
}
void pa_print_sinks() {
    pa_wait_for_ready();

    pa_context* context = pa_get_context();
    pa_operation *pa_op = pa_context_get_sink_info_list(
            context, pa_sinklist_cb, NULL);

    pa_wait_for_operation(pa_op);
}

void pa_source_cb(pa_context *c, const pa_source_info *l, 
        int eol, void *userdata) {
    if (eol != 0 || l == NULL)
        return;

    std::cout << "Source: " << l->description << std::endl;
}
void pa_print_sources() {
    pa_wait_for_ready();

    pa_context* context = pa_get_context();
    pa_operation *pa_op = pa_context_get_source_info_list(
            context, pa_source_cb, NULL);

    pa_wait_for_operation(pa_op);
}
