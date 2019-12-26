/**
 * @file adapter_example.c
 *
 * @brief Adapter example code for VL53L1 ToF sensors.
 *
 * @author Lucas Schneider <lucas.schneider@thunderatz.org>
 * @author Bernardo Coutinho <bernardo.coutinho@thunderatz.org>
 *
 * @date 11/2019
 */

#include <stdbool.h>
#include <stdint.h>

#include "adapter_example.h"

#include "vl53l1.h"

#include "main.h"
#include "i2c.h"

/*****************************************
 * Private Constants
 *****************************************/

#define VL53L1_DEFAULT_COMM_SPEED_KHZ 100

#define INIT_RESET_SLEEP_TIME_MS 10
#define MAX_RANGE 2000

/*****************************************
 * Private Variables
 *****************************************/

static VL53L1_Dev_t sensors[] = {
    { // 0
        .I2cDevAddr = VL53L1_DEFAULT_ADDRESS,
        .comms_type = 1, // I2C
        .comms_speed_khz = VL53L1_DEFAULT_COMM_SPEED_KHZ,
        .present = 0,
        .calibrated = 0,
        .I2cHandle = I2C_HANDLER,
        .xshut_port = FIRST_SENSOR_GPIOx,
        .xshut_pin = FIRST_SENSOR_GPIO_PIN
    },
    { // 1
        .I2cDevAddr = VL53L1_DEFAULT_ADDRESS,
        .comms_type = 1, // I2C
        .comms_speed_khz = VL53L1_DEFAULT_COMM_SPEED_KHZ,
        .present = 0,
        .calibrated = 0,
        .I2cHandle = I2C_HANDLER,
        .xshut_port = SECOND_SENSOR_GPIOx,
        .xshut_pin = SECOND_SENSOR_GPIO_PIN
    },
    { // 2
        .I2cDevAddr = VL53L1_DEFAULT_ADDRESS,
        .comms_type = 1, // I2C
        .comms_speed_khz = VL53L1_DEFAULT_COMM_SPEED_KHZ,
        .present = 0,
        .calibrated = 0,
        .I2cHandle = I2C_HANDLER,
        .xshut_port = THIRD_SENSOR_GPIOx,
        .xshut_pin = THIRD_SENSOR_GPIO_PIN
    }
};

static VL53L1_DeviceInfo_t sensors_info[DS_AMOUNT];
static VL53L1_RangingMeasurementData_t sensors_measurement[DS_AMOUNT];
static VL53L1_CalibrationData_t sensors_calibration[DS_AMOUNT];

static uint16_t actual_range[] = {MAX_RANGE, MAX_RANGE, MAX_RANGE};
static const uint8_t used_sensors[] = {1, 1, 1};
static const uint8_t i2c_addresses[] = {130, 134, 138};
__attribute__((used)) static uint8_t sensors_status[] = {0, 0, 0};

/*****************************************
 * Public Functions Bodies Definitions
 *****************************************/

uint8_t distance_sensors_adapter_init(void) {
    TARGET_I2C_INIT();

    VL53L1_Error global_status = VL53L1_ERROR_NONE;

    // desabilita todos, independente de quantos vai usar
    for (int i = 0; i < DS_AMOUNT; i++) {
        vl53l1_turn_off(&(sensors[i]));
    }

    HAL_Delay(INIT_RESET_SLEEP_TIME_MS);

    for (int i = 0; i < DS_AMOUNT; i++) {
        if (!used_sensors[i]) {
            continue;
        }

        VL53L1_Error status = VL53L1_ERROR_NONE;
        VL53L1_Dev_t* p_device = &(sensors[i]);

        status = VL53L1_WaitDeviceBooted(p_device);

        if (status == VL53L1_ERROR_NONE) {
            status = VL53L1_SetDeviceAddress(p_device, i2c_addresses[i]);
        }

        if (status == VL53L1_ERROR_NONE) {
            p_device->I2cDevAddr = i2c_addresses[i];
            status = vl53l1_init(p_device, sensors_info[i], &sensors_calibration[i]);
        }

        if (status == VL53L1_ERROR_NONE) {
            p_device->present = 1;
            p_device->calibrated = 1;
        }

        global_status |= status;
    }

    if (global_status == VL53L1_ERROR_NONE) {
        return 0;
    }

    return 1;
}

uint8_t distance_sensors_adapter_update(void) {
    uint8_t status = 0;

    for (int i = 0; i < DS_AMOUNT; i++) {
        if (!used_sensors[i]) {
            continue;
        }

        sensors_status[i] =
            vl53l1_update_range(&(sensors[i]), &(sensors_measurement[i]), &(actual_range[i]), MAX_RANGE);

        if (sensors_status[i] != 0) {
            status |= 1 << (i + 1);
        }
    }

    return status;
}

uint16_t distance_sensors_adapter_get(distance_sensor_position_t sensor) {
    if ((sensors[(int) sensor]).present) {
        return actual_range[(int) sensor];
    }

    return -1;
}