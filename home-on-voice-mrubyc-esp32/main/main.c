#include <stdio.h>

#include "esp_system.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "nvs_flash.h"

#include "mrubyc.h"
#include "wifi.h"
#include "mqtt_broker.h"
#include "motor.h"
#include "light.h"

#include "models/motor.h"
#include "models/light.h"
#include "models/mqtt.h"
#include "loops/master.h"

#define MEMORY_SIZE (1024*40)

static uint8_t memory_pool[MEMORY_SIZE];


//================================================================
/*! DEBUG PRINT
*/
void chip_info() {
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
}

static void c_debugprint(struct VM *vm, mrbc_value v[], int argc){
  for( int i = 0; i < 79; i++ ) { console_putchar('='); }
  console_putchar('\n');
  chip_info();
  int total, used, free, fragment;
  mrbc_alloc_statistics( &total, &used, &free, &fragment );
  console_printf("Memory total:%d, used:%d, free:%d, fragment:%d\n", total, used, free, fragment );
  unsigned char *key = GET_STRING_ARG(1);
  unsigned char *value = GET_STRING_ARG(2);
  console_printf("%s:%s\n", key, value );
  heap_caps_print_heap_info(MALLOC_CAP_8BIT);
  heap_caps_print_heap_info(MALLOC_CAP_32BIT);
  for( int i = 0; i < 79; i++ ) { console_putchar('='); }
  console_putchar('\n');
}

void app_main(void) {
  nvs_flash_init();
  mrbc_init(memory_pool, MEMORY_SIZE);
  mrbc_define_method(0, mrbc_class_object, "debugprint", c_debugprint);
  mrbc_define_method(0, mrbc_class_object, "connected_to_network", c_network_connected);
  mrbc_define_method(0, mrbc_class_object, "initialise_wifi", c_wifi_init_sta);
  mrbc_define_method(0, mrbc_class_object, "mqtt_app_start", mqtt_app_start);
  mrbc_define_method(0, mrbc_class_object, "new_request", c_check_latest_updates);
  mrbc_define_method(0, mrbc_class_object, "connected_to_mqqt_broker", c_mqtt_connected);
  mrbc_define_method(0, mrbc_class_object, "received_message", c_received_message);
  mrbc_define_method(0, mrbc_class_object, "set_servo_gpio", c_servo_gpio_initialize);
  mrbc_define_method(0, mrbc_class_object, "set_servo_params", c_servo_params_initialize);
  mrbc_define_method(0, mrbc_class_object, "run_servo", c_run_servo_motor);
  mrbc_define_method(0, mrbc_class_object, "send_notification", c_send_notification);
  mrbc_define_method(0, mrbc_class_object, "turn_light_on", rmt_example_nec_tx_task);
  mrbc_define_method(0, mrbc_class_object, "init_rmt_config", setup_rmt_config);
  mrbc_create_task(motor, 0);
  mrbc_create_task(light, 0);
  mrbc_create_task(mqtt, 0);
  mrbc_create_task(master, 0);
  mrbc_run();
}
