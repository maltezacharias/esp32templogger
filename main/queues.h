#include "freertos/queue.h"
#include "freertos/semphr.h"

/* Queue for Sensor Values */
extern QueueHandle_t sensorValueQueue;
typedef struct sensorMessages {
    float temperature;
} sensorMessage;

/* Queue for notification of new picture available in PSRAM */
extern QueueHandle_t pictureQueue;

/* Queue for switching the relay */
extern QueueHandle_t relayQueue;
typedef struct relayMessages {
    bool state;
} relayMessage;

extern SemaphoreHandle_t pictureSemaphore;