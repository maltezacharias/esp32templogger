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
    ESP_LOGI(TAG,"Starting App Main");
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

    for (;;) {
        sensorMessage sensorData;
        BaseType_t receiveReturn = xQueueReceive(sensorValueQueue, &sensorData, portMAX_DELAY);
        if (receiveReturn == pdTRUE) {
            ESP_LOGI(TAG,"Received sensor data");
            for (int i = 0; i < TEMPERATURE_SLOTS; i++) {
                ESP_LOGI(TAG,"Sensor %i Temperature %.1f degrees Celsius (ERROR: %i)",i , sensorData.temperature[i],sensorData.errorCode[i]);
            }
        }
    }
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

    ESP_LOGD(
        TAG,
        "Remaining Stack: Printer: %5.i  Sensor: %5.i  Camera: %5.i  Relay: %5.i  Send Picture: %5.i",
        freeStackPrinter,
        freeStackSensor,
        freeStackCamera,
        freeStackRelay,
        freeStackSendPicture
    );
}
