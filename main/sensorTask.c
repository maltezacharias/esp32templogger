#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "owb.h"
#include "owb_rmt.h"
#include "ds18b20.h"
#include "globals.h"
#include "sensorTask.h"

OneWireBus * owb;
DS18B20_Info * sensors[MAX_DEVICES] = {0};
int sensorsCount=0;
static owb_rmt_driver_info rmt_driver_info;
static const char* TAG = "sensors";

static void initialize();
static void loop();
static void cleanup();

void sensorTask(void *pvParameter) {

    initialize();

    if (sensorsCount == 0) {
        ESP_LOGW(TAG,"No sensors detected. Stopping sensor task");
        cleanup();
        vTaskDelete(NULL);
        return;
    }

    TickType_t last_wake_time = xTaskGetTickCount();
    for (;;) {
        loop();
        vTaskDelayUntil(&last_wake_time, SAMPLE_PERIOD / portTICK_PERIOD_MS);
    }
    
    // Never reached, leaving it here so I don't forget it, if I later implement sensor redetection/task stopping
    cleanup();
}

static void initialize() {
    // Stable readings require a brief period before communication
    vTaskDelay(2000.0 / portTICK_PERIOD_MS);

    // Create a 1-Wire bus, using the RMT timeslot driver
    owb = owb_rmt_initialize(&rmt_driver_info, GPIO_DS18B20_0, RMT_CHANNEL_1, RMT_CHANNEL_0);
    owb_use_crc(owb, true);  // enable CRC check for ROM code

    sensorsCount = 0;
    OneWireBus_SearchState search_state = {0};
    bool found = false;
    owb_search_first(owb, &search_state, &found);
    while (found)
    {
        DS18B20_Info * ds18b20_info = ds18b20_malloc();  // heap allocation
        sensors[sensorsCount] = ds18b20_info;
        ds18b20_init(ds18b20_info, owb, search_state.rom_code);
        ds18b20_use_crc(ds18b20_info, true);           // enable CRC check on all reads
        ds18b20_set_resolution(ds18b20_info, DS18B20_RESOLUTION);
        sensorsCount++;
        owb_search_next(owb, &search_state, &found);
    }
    ESP_LOGI(TAG,"Found %d device%s", sensorsCount, sensorsCount == 1 ? "" : "s");
    
    for (int sensor=0; sensor < MAX_DEVICES;sensor++) {
        char rom_code_s[17]; // 16 Chars + Terminator
        owb_string_from_rom_code(sensors[sensor]->rom_code, rom_code_s, sizeof(rom_code_s));
        ESP_LOGD(TAG,"SENSOR_DUMP: %d:(%s)", sensor, rom_code_s);   
    }        

    bool parasitic_power = false;
    ds18b20_check_for_parasite_power(owb, &parasitic_power);
    if (parasitic_power) {
        ESP_LOGI(TAG,"Parasitic-powered devices detected");
    }
    // In parasitic-power mode, devices cannot indicate when conversions are complete,
    // so waiting for a temperature conversion must be done by waiting a prescribed duration
    owb_use_parasitic_power(owb, parasitic_power);

#ifdef CONFIG_ENABLE_STRONG_PULLUP_GPIO
    // An external pull-up circuit is used to supply extra current to OneWireBus devices
    // during temperature conversions.
    owb_use_strong_pullup_gpio(owb, CONFIG_STRONG_PULLUP_GPIO);
#endif
}

static void loop() {
    ds18b20_convert_all(owb);
    // In this application all devices use the same resolution,
    // so use the first device to determine the delay
    ds18b20_wait_for_conversion(sensors[0]);

    // Read the results immediately after conversion otherwise it may fail
    // (using printf before reading may take too long)
    sensorMessage sensorData = { .temperature = {0}, .errorCode = {0} };
    
    for (int i = 0; i < sensorsCount; ++i) {
        sensorData.errorCode[i] = ds18b20_read_temp(sensors[i], &sensorData.temperature[i]);
    }

    // Print results in a separate loop, after all have been read
    char rom_code_s[17]; // 16 Chars + Terminator
    
    for (int i = 0; i < sensorsCount; ++i) {
        owb_string_from_rom_code(sensors[i]->rom_code, rom_code_s, sizeof(rom_code_s));
        ESP_LOGD(TAG,"Sensor %i(%s) Temperature %.1f degrees Celsius (ERROR: %i)",i,rom_code_s, sensorData.temperature[i],sensorData.errorCode[i]);
    }

    ESP_LOGD(TAG,"Sending temperature via Queue");
}

static void cleanup() {
    // clean up dynamically allocated data
    for (int i = 0; i < sensorsCount; ++i) {
        ds18b20_free(&sensors[i]);
    }
    owb_uninitialize(owb); 
}

