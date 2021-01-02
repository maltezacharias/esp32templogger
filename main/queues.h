#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "ds18b20.h"
#define TEMPERATURE_SLOTS (CONFIG_MAX_SENSORS)

/* Queue for Sensor Values */
extern QueueHandle_t sensorValueQueue;
typedef struct {
    float temperature[TEMPERATURE_SLOTS];
    DS18B20_ERROR errorCode[TEMPERATURE_SLOTS];
} sensorMessage;

/* Queue for notification of new picture available in PSRAM */
extern QueueHandle_t pictureQueue;

/* Queue for switching the relay */
extern QueueHandle_t relayQueue;
typedef struct {
    bool state;
} relayMessage;

extern SemaphoreHandle_t pictureSemaphore;