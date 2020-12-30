/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
static const char* TAG = "main";

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "collect_temperatures.h"
#include "globals.h"

#include "freertos/queue.h"
#include "queues.h"

#include "camTask.h"
#include "switchTask.h"
void printTask(void * taskParameters);

void initializeQueues() {
    sensorValueQueue = xQueueCreate( 1,sizeof(sensorMessage) );
    pictureQueue = xQueueCreate(1, sizeof(bool));
    switchQueue = xQueueCreate(1, sizeof(switchMessage));
}


void app_main(void) {
    ESP_LOGI(TAG,"Starting Tasks");
    TaskHandle_t printTaskHandle, sensorTaskHandle, cameraTaskHandle, switchTaskHandle;

    xTaskCreate(printTask,"printer",2048,NULL,5,&printTaskHandle);
    xTaskCreate(sensor_task,"sensor",2048,NULL,5,&sensorTaskHandle);
    xTaskCreate(cameraTask,"camera",4096,NULL,5,&cameraTaskHandle);
    xTaskCreate(switchingTask,"switch",2048,NULL,4,&switchTaskHandle);

    for (;;) {
     
        UBaseType_t freeStackPrinter, freeStackSensor, freeStackCamera,freeStackSwitch;
        freeStackPrinter = uxTaskGetStackHighWaterMark( printTaskHandle );
        freeStackSensor = uxTaskGetStackHighWaterMark( sensorTaskHandle );
        freeStackCamera = uxTaskGetStackHighWaterMark( cameraTaskHandle );
        freeStackSwitch = uxTaskGetStackHighWaterMark( switchTaskHandle );

        ESP_LOGI(
            TAG,
            "Remaining Stack: Printer: %5.i  Sensor: %5.i  Camera: %5.i  Switch: %5.i",
            freeStackPrinter,
            freeStackSensor,
            freeStackCamera,
            freeStackSwitch
        );
        vTaskDelay( 1000 / portTICK_PERIOD_MS);
    }
}   


void printTask(void * taskParameters) {
    while(true) {

        for (int sensor=0; sensor < MAX_DEVICES;sensor++) {
            char rom_code_s[17]; // 16 Chars + Terminator
            owb_string_from_rom_code(sensors[sensor], rom_code_s, sizeof(rom_code_s));
            ESP_LOGI(TAG,"SENSOR_DUMP: %d:(%s)", sensor, rom_code_s);   
        }
        vTaskDelay(5000.0 / portTICK_PERIOD_MS);
    }
}
