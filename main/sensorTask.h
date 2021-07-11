#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ds18b20.h"

#define GPIO_DS18B20_0       (CONFIG_ONE_WIRE_GPIO)
#define MAX_DEVICES          (CONFIG_MAX_SENSORS)
#define DS18B20_RESOLUTION   (DS18B20_RESOLUTION_12_BIT)
#define SAMPLE_PERIOD        (CONFIG_TEMPERATURE_SAMPLE_PERIOD)   // milliseconds

extern DS18B20_Info * sensors[];
void sensorTask(void *pvParameter);