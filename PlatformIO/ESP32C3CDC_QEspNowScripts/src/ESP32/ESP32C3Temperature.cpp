/* Temperature Sensor Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <Arduino.h>

/* Note: ESP32 don't support temperature sensor */
#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3
#include "driver/temp_sensor.h"

/**
 * @brief readTempSensor()
 * 
 * @return float, tempSensor data, -10~80°C 
 */
float readTempSensor()
{
    float tsens_out;
    temp_sensor_read_celsius(&tsens_out);

    return tsens_out;
}

void setupTempSensor()
{
    // Initialize touch pad peripheral, it will start a timer to run a filter
    log_v("Initializing Temperature sensor");
    float tsens_out;
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    temp_sensor_get_config(&temp_sensor);
    //log_v("default dac %d, clk_div %d", temp_sensor.dac_offset, temp_sensor.clk_div);
    temp_sensor.dac_offset = TSENS_DAC_DEFAULT; // DEFAULT: range:-10℃ ~  80℃, error < 1℃.
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
    log_i("Temperature sensor(-10℃ ~ 80℃) started...");

}


#else
#error TempSensor N/A
#endif