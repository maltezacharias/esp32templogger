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
#include "globals.h"

#include "freertos/queue.h"
#include "queues.h"
#include "freertos/semphr.h"

#include "sensorTask.h"
#include "camTask.h"
#include "relayTask.h"

#define DEFAULT_TASK_STACK_SIZE 4096

static struct  {
    TaskHandle_t print;
    TaskHandle_t sensor;
    TaskHandle_t camera;
    TaskHandle_t relay;
    TaskHandle_t sendPicture;
} taskHandles;

void printTask(void * taskParameters);
void sendPictureTask(void * TaskParameters);
void logStackStatistics();

// Define Queues and Semaphore from queues.h and allocate memory
QueueHandle_t sensorValueQueue;
QueueHandle_t pictureQueue;
QueueHandle_t relayQueue;
SemaphoreHandle_t pictureSemaphore;

void initializeQueues() {
    sensorValueQueue = xQueueCreate( 1,sizeof(sensorMessage) );
    pictureQueue = xQueueCreate(1, sizeof(bool));
    relayQueue = xQueueCreate(1, sizeof(relayMessage));
    pictureSemaphore = xSemaphoreCreateBinary();
}


void app_main(void) {

    ESP_LOGI(TAG,"Initializing Queues");
    initializeQueues();

    ESP_LOGI(TAG,"Starting Tasks");
    xTaskCreate(printTask,"printer",DEFAULT_TASK_STACK_SIZE,NULL,5,&taskHandles.print);
    xTaskCreate(sensorTask,"sensor",DEFAULT_TASK_STACK_SIZE,NULL,5,&taskHandles.sensor);
    xTaskCreate(cameraTask,"camera",DEFAULT_TASK_STACK_SIZE,NULL,5,&taskHandles.camera);
    xTaskCreate(relayTask,"relay",DEFAULT_TASK_STACK_SIZE,NULL,4,&taskHandles.relay);
    xTaskCreate(sendPictureTask,"picture",DEFAULT_TASK_STACK_SIZE,NULL,4,&taskHandles.sendPicture);

    vTaskDelay(10000 / portTICK_PERIOD_MS);
    logStackStatistics();
}   


void printTask(void * taskParameters) {
    vTaskDelay(5000.0 / portTICK_PERIOD_MS);
    for (int sensor=0; sensor < MAX_DEVICES;sensor++) {
        char rom_code_s[17]; // 16 Chars + Terminator
        owb_string_from_rom_code(sensors[sensor]->rom_code, rom_code_s, sizeof(rom_code_s));
        ESP_LOGI(TAG,"SENSOR_DUMP: %d:(%s)", sensor, rom_code_s);   
    }        
    vTaskDelete(NULL);
}

void sendPictureTask(void * TaskParameters) {
    for  (;;) {
        ESP_LOGI("main:sendPicture", "Trying to get Semaphore and simulate sending");
        xSemaphoreTake(pictureSemaphore,portMAX_DELAY);
        ESP_LOGI("main:sendPicture", "Got picture, sending!");
        vTaskDelay(300 / portTICK_PERIOD_MS);
        ESP_LOGI("main:sendPicture", "Picture sent, releasing semaphor");
        xSemaphoreGive(pictureSemaphore);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void logStackStatistics() {
    UBaseType_t freeStackPrinter, freeStackSensor, freeStackCamera,freeStackRelay,freeStackSendPicture;
    freeStackPrinter     = uxTaskGetStackHighWaterMark( taskHandles.print );
    freeStackSensor      = uxTaskGetStackHighWaterMark( taskHandles.sensor );
    freeStackCamera      = uxTaskGetStackHighWaterMark( taskHandles.camera );
    freeStackRelay       = uxTaskGetStackHighWaterMark( taskHandles.relay );
    freeStackSendPicture = uxTaskGetStackHighWaterMark( taskHandles.sendPicture );

    ESP_LOGI(
        TAG,
        "Remaining Stack: Printer: %5.i  Sensor: %5.i  Camera: %5.i  Relay: %5.i  Send Picture: %5.i",
        freeStackPrinter,
        freeStackSensor,
        freeStackCamera,
        freeStackRelay,
        freeStackSendPicture
    );
}