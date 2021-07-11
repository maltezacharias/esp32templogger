idf_component_register(
    SRCS "wifi.c" "app_main.c" "relayTask.c" "camTask.c" "mqttTask.c" "sensorTask.c"
    INCLUDE_DIRS ""
)

target_compile_options(${COMPONENT_LIB} PRIVATE -D LOG_LOCAL_LEVEL=ESP_LOG_INFO)