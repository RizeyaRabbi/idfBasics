#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/ledc.h"

#define log "log"
#define led 2

void ledBlink5Times()
{
    // #include "freertos/FreeRTOS.h"
    // #include "freertos/task.h"
    // #include "driver/gpio.h"
    // #include "esp_log.h"
    esp_rom_gpio_pad_select_gpio(led);
    gpio_set_direction(led, GPIO_MODE_OUTPUT);
    for (int i = 0; i < 5; i++)
    {
        gpio_set_level(led, 1);
        ESP_LOGI(log, "Led Turned ON");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(led, 0);
        ESP_LOGI(log, "Led Turned OFF");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/*
void takeKeyboardInput()
{
    char character = 0;
    char string[50];
    memset(string, 0, sizeof(string));
    printf("Enter string..");
    while (character != '\n')
    {
        character = getchar();
        if (character != 0xff)
        {
            string[strlen(string)] = character;
        }
    }
    printf("String is: %s", string);
}
*/
void espLOG()
{
    // #include "esp_log.h"
    //  ESP-IDF supports 5 log levels
    //  ESP_LOGE : Error Logging (lowest level)
    //  ESP_LOGW : Warning Logging
    //  ESP_LOGI : Informational Log
    //  ESP_LOGD : Debug Statement
    //  ESP_LOGV : Verbose(highest level)
    /*
    ESP_LOGE(LOG,"Error log");(lowest level)
    ESP_LOGW(LOG,"Warning log");
    ESP_LOGI(LOG,"Information log");
    ESP_LOGD(LOG,"Debug Log");
    ESP_LOGV(LOG,"Verbose log");(highest level)
    */

    // By default debug and verbose are not turned ON.
    // We need to turn ON manually.
    ESP_LOGE("LOG", "Error log");
    ESP_LOGW("LOG", "Warning log");
    ESP_LOGI("LOG", "Information log");
    ESP_LOGD("LOG", "Debug Log");
    ESP_LOGV("LOG", "Verbose log");

    // We can set the log level in CODE
    // This will print with TAG "LOGtest" till LOGI(In this code first three)
    esp_log_level_set("LOGtest", ESP_LOG_INFO);
    ESP_LOGE("LOGtest", "Error log");
    ESP_LOGW("LOGtest", "Warning log");
    ESP_LOGI("LOGtest", "Information log");
    ESP_LOGD("LOGtest", "Debug Log");
    ESP_LOGV("LOGtest", "Verbose log");

    // This will print only information log for the tag logTest
    esp_log_level_set("logTest", ESP_LOG_INFO);
    ESP_LOGI("logTest", "test Information log");

    // log TAG is defined in macros
    // This portion of code will delay 1000ms and print five times
    for (int i = 0; i < 5; i++)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGI(log, "loop %d", i);
    }
}

void digitalInputAndOutput()
{
    // #include "freertos/FreeRTOS.h"
    // #include "freertos/task.h"
    // #include "driver/gpio.h"
    const int outputPin = 2;
    const int inputPin = 25;
    bool status = false;
    gpio_set_direction(outputPin, GPIO_MODE_OUTPUT);
    gpio_set_direction(inputPin, GPIO_MODE_INPUT);
    gpio_pullup_en(inputPin);
    gpio_pulldown_dis(inputPin);
    while (1)
    {
        if (!status)
        {
            if (gpio_get_level(inputPin) == 1)
            {
                gpio_set_level(outputPin, 1);
                ESP_LOGI(log, "Led Turned ON");
                status = true;
            }
        }

        if (status)
        {
            if (gpio_get_level(inputPin) == 0)
            {
                gpio_set_level(outputPin, 0);
                ESP_LOGI(log, "Led Turned OFF");
                status = false;
            }
        }
        vTaskDelay(1);
    }
}

void analogInputWithCalibraton()
{
    // #include "driver/adc.h"
    // #include "esp_adc_cal.h"
    // #include <stdio.h>
    // #include <stdlib.h>
    // #include "freertos/FreeRTOS.h"
    // #include "freertos/task.h"

    /*
       ADC1 Pins
       ADC1_CH0 :GPIO 36
       ADC1_CH1 :GPIO 37  (NOT AVAILABLE)
       ADC1_CH2 :GPIO 38  (NOT AVAILABLE)
       ADC1_CH3 :GPIO 39
       ADC1_CH4 :GPIO 32
       ADC1_CH5 :GPIO 33
       ADC1_CH6 :GPIO 34
       ADC1_CH7 :GPIO 35

       ADC2 Pins
       ADC2_CH0 :GPIO 4
       ADC2_CH1 :GPIO 0
       ADC2_CH2 :GPIO 2
       ADC2_CH3 :GPIO 15
       ADC2_CH4 :GPIO 13
       ADC2_CH5 :GPIO 12
       ADC2_CH6 :GPIO 14
       ADC2_CH7 :GPIO 27
       ADC2_CH8 :GPIO 25
       ADC2_CH9 :GPIO 26
    */
    /*
    Attenuation	Attenuation Parameter	Input Voltage Range
            0dB	      ADC_ATTEN_DB_0	~100-950 mV
            2.5dB	  ADC_ATTEN_DB_2_5	~100-1250mV
            6dB	      ADC_ATTEN_DB_6	~150-1750mV
            11dB	  ADC_ATTEN_DB_11	~150-2450mV
    */
    /*
    ADC_WIDTH_BIT_DEFAULT (12-bit)
    ADC_WIDTH_BIT_9
    ADC_WIDTH_BIT_10
    ADC_WIDTH_BIT_11
    ADC_WIDTH_BIT_12
    */

    // ADC calibration
    static esp_adc_cal_characteristics_t adc1_chars; // adc1_chars will hold the adc calibrated value
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);

    // Setting ADC bit width
    adc1_config_width(ADC_WIDTH_BIT_DEFAULT);

    // Setting input voltage range
    adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11);
    int voltage;
    while (1)
    {
        // Getting RAW adc value
        int adcValue = adc1_get_raw(ADC1_CHANNEL_4);
        ESP_LOGI(log, "Raw Value: %d", adcValue);

        // Converting adc value to voltage
        voltage = esp_adc_cal_raw_to_voltage(adcValue, &adc1_chars);
        ESP_LOGI(log, "Voltage: %d", voltage);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void analogInputWithoutCalibration()
{
    // #include <stdio.h>
    // #include <stdlib.h>
    // #include "freertos/FreeRTOS.h"
    // #include "freertos/task.h"
    // #include "driver/adc.h"

    adc1_channel_t adc_channel = ADC1_CHANNEL_4;
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(adc_channel, ADC_ATTEN_DB_11);
    while (1)
    {
        int value = adc1_get_raw(adc_channel);
        ESP_LOGI(log, "Raw Value: %d", value);

        double voltage = value * 0.0008057;
        ESP_LOGI(log, "Voltage: %lf", voltage);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void ledPWM()
{
    /*
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/ledc.h"
   */
    adc1_channel_t adcChannel = ADC1_CHANNEL_4;
    adc1_config_width(ADC_WIDTH_BIT_10);
    adc1_config_channel_atten(adcChannel, ADC_ATTEN_DB_11);

    const int ledGPIO = 2;
    ledc_channel_config_t ledcChannel;
    ledc_timer_config_t ledcTimer = {
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = 1000,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledcTimer);
    ledcChannel.channel = LEDC_CHANNEL_0;
    ledcChannel.duty = 0;
    ledcChannel.gpio_num = ledGPIO;
    ledcChannel.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledcChannel.hpoint = 0;
    ledcChannel.timer_sel = LEDC_TIMER_0;
    ledc_channel_config(&ledcChannel);

    while (1)
    {
        int adcValue = 0;
        for (int i = 0; i < 30; i++)
        {
            adcValue += adc1_get_raw(adcChannel);
        }
        adcValue /= 30;
        ESP_LOGI(log, "Average Value: %d", adcValue);

        ledc_set_duty(ledcChannel.speed_mode, ledcChannel.channel, adcValue);
        ledc_update_duty(ledcChannel.speed_mode, ledcChannel.channel);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
void app_main(void)
{
    // espLOG();
    // ledBlink5Times();
    // digitalInputAndOutput();
    // analogInputWithCalibraton();
    // analogInputWithoutCalibration();
    ledPWM();
}
