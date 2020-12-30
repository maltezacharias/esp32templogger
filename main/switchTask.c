#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "switchTask.h"
#include "driver/gpio.h"

static const char* TAG = "switch";

void switchingTask(void *taskParameters) {

    uint32_t level = 0;
    gpio_config_t config = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
        .pin_bit_mask = 0LL
    };
    config.pin_bit_mask |= 1LL << GPIO_SWITCH;

    ESP_LOGI(TAG, "Configuring GPIO");
    gpio_config(&config);

    for (;;) {
        ESP_LOGI(TAG, "Toggling Switch GPIO");
        gpio_set_level(GPIO_SWITCH, level);
        level ^= 1; // flip level        
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

}