// ESP IDF ESP32 remote ctrol transmitter Example

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "mrubyc.h"


static const char* LOG_TAG = "TEST_LOG";
//light: 41B6D52A ac: F20D03FC tv: 1A2EEC3B
static const uint32_t LIGHT_ON =  0x41B6D52A;
static const uint32_t LIGHT_OFF = 0x41B6D52A;
const rmt_channel_t RMT_CHANNEL = 0;
const gpio_num_t IR_PIN = 19;
const int  RMT_TX_CARRIER_EN = 1;   // Enable carrier for IR transmitter test with IR led

const int leaderOnUs = 9000;
const int leaderOffUs = 4500;
const int dataOnUs = 560;
const int data1OffUs = 1690;
const int data0OffUs = 560;
const int stopbitOnUs = 560;
const int stopbitOffUs = 0x7fff;

// send NEC format remote control data
void sendNECRCData(uint32_t sendCode) {
  const int sendDataLength = 34;        // NEC format data length 34 bit
  rmt_item32_t sendData[sendDataLength]; // data to send
  uint16_t customCode = (uint16_t)((sendCode>>16) & 0x0000FFFF) ;
  uint8_t dataCode = (uint8_t)(((sendCode & 0x0000FF00)>>8) & 0x000000FF) ;

  // Leader code
  sendData[0].duration0 = leaderOnUs;
  sendData[0].level0 = 1;
  sendData[0].duration1 = leaderOffUs;
  sendData[0].level1 = 0;

  //Customer Code
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 8; j++) {
      sendData[8 * i + j + 1].duration0 = dataOnUs;
      sendData[8 * i + j + 1].level0 = 1;
      if (customCode & (1 << ((1-i) * 8 + (7-j)))) {
        // 1: ON 560us + OFF 1690us
        sendData[8 * i + j + 1].duration1 = data1OffUs;
      } else {
        // 0: ON 560us + OFF  560us
        sendData[8 * i + j + 1].duration1 = data0OffUs;
      }
      sendData[8 * i + j + 1].level1 = 0;
    }
  }


  // Data code
  for (int i = 0; i < 8; i++) {
    sendData[i + 17].duration0 = dataOnUs;
    sendData[i + 25].duration0 = dataOnUs;
    sendData[i + 17].level0 = 1;
    sendData[i + 25].level0 = 1;
    if (dataCode & (1 << (7-i))) {
      sendData[i + 17].duration1 = data1OffUs;
      sendData[i + 25].duration1 = data0OffUs;
    } else {
      sendData[i + 17].duration1 = data0OffUs;
      sendData[i + 25].duration1 = data1OffUs;
    }
    sendData[i + 17].level1 = 0;
    sendData[i + 25].level1 = 0;
  }

  /* stop bit 1bit: ON 560 */
  sendData[33].duration0 = stopbitOnUs;
  sendData[33].level0 = 1;
  sendData[33].duration1 = stopbitOffUs;
  sendData[33].level1 = 0;

  rmt_write_items(RMT_CHANNEL, sendData, sendDataLength, true);
  rmt_wait_tx_done(RMT_CHANNEL, portMAX_DELAY); // wait tx (no limit)

}

void setup_rmt_config() {
  // put your setup code here, to run once:
  rmt_config_t rmtConfig;

  rmtConfig.rmt_mode = RMT_MODE_TX;  // transmit mode
  rmtConfig.channel = RMT_CHANNEL;  // channel to use 0 - 7
  rmtConfig.clk_div = 80;  // clock divider 1 - 255. source clock is 80MHz -> 80MHz/80 = 1MHz -> 1 tick = 1 us
  rmtConfig.gpio_num = IR_PIN; // pin to use
  rmtConfig.mem_block_num = 1; // memory block size

  rmtConfig.tx_config.loop_en = 0; // no loop
  rmtConfig.tx_config.carrier_freq_hz = 38000;  // IR remote controller uses 38kHz carrier frequency
  rmtConfig.tx_config.carrier_duty_percent = 33; // duty
  rmtConfig.tx_config.carrier_level =  RMT_CARRIER_LEVEL_HIGH; // carrier level
  rmtConfig.tx_config.carrier_en = RMT_TX_CARRIER_EN;  // carrier enable
  rmtConfig.tx_config.idle_level =  RMT_IDLE_LEVEL_LOW ; // signal level at idle
  rmtConfig.tx_config.idle_output_en = true;  // output if idle

  rmt_config(&rmtConfig);
  rmt_driver_install(rmtConfig.channel, 0, 0);
}

void rmt_example_nec_tx_task()
{
    // vTaskDelay(10);
    // setup_rmt_config();
    esp_log_level_set(LOG_TAG, ESP_LOG_INFO);
    uint32_t send_data = LIGHT_ON;

    // while(1) {
      ESP_LOGI(LOG_TAG, "SEND RMT DATA");
      ESP_LOGI(LOG_TAG, "LIGHT");

      sendNECRCData(send_data);
      if(send_data == LIGHT_ON) {
        send_data = LIGHT_OFF;
      }
      else {
        send_data = LIGHT_ON;
      // }

      // vTaskDelay(5000 / portTICK_PERIOD_MS); //10 sec delay

    }

    // vTaskDelete(NULL);
}

static void c_switch_light(struct VM *vm, mrbc_value v[], int argc) {
  int on = GET_INT_ARG(1);
  int data = LIGHT_OFF;
  if(on == 1) {
    data = LIGHT_ON;
  }
  sendNECRCData(data);
}

// void app_main()
// {
//     xTaskCreate(rmt_example_nec_tx_task, "rmt_nec_tx_task", 2048, NULL, 10, NULL);
// }
