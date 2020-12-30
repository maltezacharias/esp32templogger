#include "freertos/queue.h"

/* Queue for Sensor Values */
extern QueueHandle_t sensorValueQueue;
typedef struct sensorMessages {
    float temperature;
} sensorMessage;

/* Queue for notification of new picture available in PSRAM */
extern QueueHandle_t pictureQueue;

/* Queue for switching the relay */
extern QueueHandle_t switchQueue;
typedef struct switchMessages {
    bool state;
} switchMessage;
