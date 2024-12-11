#include "pulse_audio_actions.h"
#include <list>

#include "pulse_audio.h"

void open_pavucontrol() {
    system("pavucontrol");
}

void pa_sinklist_cb(pa_context *c, const pa_sink_info *l, 
        int eol, void *userdata) {
    if (eol != 0 || l == NULL)
        return;

    std::list <pa_sink_info*> *sinks 
        = (std::list <pa_sink_info*> *) userdata;

    sinks->push_back((pa_sink_info*) l);
}

std::list <pa_sink_info*> pa_get_sinks() {
    std::list <pa_sink_info*> sinks;
    pa_wait_for_ready();

    pa_context* context = pa_get_context();
    pa_operation *pa_op = pa_context_get_sink_info_list(
            context, pa_sinklist_cb, &sinks);

    pa_wait_for_operation(pa_op);

    return sinks;
}

void pa_source_cb(pa_context *c, const pa_source_info *l, 
        int eol, void *userdata) {
    if (eol != 0 || l == NULL)
        return;

    std::list <pa_source_info*> *sources
        = (std::list <pa_source_info*> *) userdata;

    sources->push_back((pa_source_info*)l);
}

std::list<pa_source_info*> pa_get_sources() {
    std::list <pa_source_info*> sources;
    pa_wait_for_ready();

    pa_context* context = pa_get_context();
    pa_operation *pa_op = pa_context_get_source_info_list(
            context, pa_source_cb, &sources);

    pa_wait_for_operation(pa_op);
    return sources;
}
