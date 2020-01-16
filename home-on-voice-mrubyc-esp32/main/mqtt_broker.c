/* MQTT over Websockets Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "mrubyc.h"
#include "mqtt_broker.h"

static const char *TAG = "MQTT_LOG";
static int new_changes = 0;
static int connected = 0;
static char message[100];
static char topic[100];
esp_mqtt_client_handle_t client;
int msg_id;

/* wait for new requests */
void c_check_latest_updates(mrb_vm *vm, mrb_value *v, int argc)
{
  if( new_changes == 1 ){  //new data arrived
    SET_TRUE_RETURN();
    new_changes = 0;
    return;
  }
  SET_FALSE_RETURN();
}

/* received message when a new request has arrived */
void c_received_message(mrb_vm *vm, mrb_value *v, int argc)
{
  mrb_value result = mrbc_string_new(vm, message, strlen(message));
  SET_RETURN(result);
  return;
}

void c_mqtt_connected(mrb_vm *vm, mrb_value *v, int argc)
{
  if( connected == 1 ){
    SET_TRUE_RETURN();
    return;
  }
  SET_FALSE_RETURN();
}

/* send a notification back to sinatra side to confirm the connection success */
void c_send_notification(mrb_vm *vm, mrb_value *v, int argc)
{
    unsigned char *status = GET_STRING_ARG(1);
    msg_id = esp_mqtt_client_publish(client, "/response", (char *) status, 0, 0, 0);
    ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    client = event->client;
    int msg_id;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_subscribe(client, "/request", 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            connected = 1;
            msg_id = esp_mqtt_client_subscribe(client, "/response", 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            connected = 0;
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            memset(message, 0, 100);
            strncpy(message, event->data, event->data_len);
            strncpy(topic, event->topic, event->topic_len);
            if(strcmp(topic, "/request") == 0){
              new_changes = 1;
            }
            memset(topic, 0, 100);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

void mqtt_app_start(mrb_vm *vm, mrb_value *v, int argc)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = CONFIG_ESP_MQTT_URI,
        .password = CONFIG_ESP_MQTT_PASSWORD,
        .username = CONFIG_ESP_MQTT_USERNAME,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}
