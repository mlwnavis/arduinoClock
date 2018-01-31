// Licence
//

typedef void (*handler_f)();
// typedef handler_f (knob_handlers_t[2]);

typedef struct knob_handlers {
  handler_f short_to_edit;
  handler_f long_to_edit;
  handler_f short_from_edit;
  handler_f long_from_edit;
} knob_handlers_t;

void knob_init();

void knob_check(knob_handlers_t);
