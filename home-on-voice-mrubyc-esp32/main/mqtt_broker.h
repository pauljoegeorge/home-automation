#include "mrubyc.h"

void mqtt_app_start(mrb_vm *vm, mrb_value *v, int argc);
void c_mqtt_connected(mrb_vm *vm, mrb_value *v, int argc);
void c_received_message(mrb_vm *vm, mrb_value *v, int argc);
void c_check_latest_updates(mrb_vm *vm, mrb_value *v, int argc);
void c_send_notification(mrb_vm *vm, mrb_value *v, int argc);
