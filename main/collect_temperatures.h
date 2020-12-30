

#define GPIO_DS18B20_0       (CONFIG_ONE_WIRE_GPIO)
#define MAX_DEVICES          (CONFIG_MAX_SENSORS)
#define DS18B20_RESOLUTION   (DS18B20_RESOLUTION_12_BIT)
#define SAMPLE_PERIOD        (1000)   // milliseconds


void sensor_task(void *pvParameter);