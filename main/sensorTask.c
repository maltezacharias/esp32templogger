#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "owb.h"
#include "owb_rmt.h"
#include "ds18b20.h"
#include "globals.h"
#include "sensorTask.h"

OneWireBus_ROMCode sensors[MAX_DEVICES]={0};
int sensorsCount=0;
static const char* TAG = "sensors";

void sensorTask(void *pvParameter)
{
     // Stable readings require a brief period before communication
    vTaskDelay(2000.0 / portTICK_PERIOD_MS);

    // Create a 1-Wire bus, using the RMT timeslot driver
    OneWireBus * owb;
    owb_rmt_driver_info rmt_driver_info;
    owb = owb_rmt_initialize(&rmt_driver_info, GPIO_DS18B20_0, RMT_CHANNEL_1, RMT_CHANNEL_0);
    owb_use_crc(owb, true);  // enable CRC check for ROM code

    sensorsCount = 0;
    OneWireBus_SearchState search_state = {0};
    bool found = false;
    owb_search_first(owb, &search_state, &found);
    while (found)
    {
        sensors[sensorsCount] = search_state.rom_code;
        sensorsCount++;
        owb_search_next(owb, &search_state, &found);
    }
    ESP_LOGI(TAG,"Found %d device%s", sensorsCount, sensorsCount == 1 ? "" : "s");

    for (;;) vTaskDelay(500 / portTICK_PERIOD_MS);
    /*vTaskDelete(NULL);*/

}