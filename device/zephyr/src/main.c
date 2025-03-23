#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sensor/lsm6dsl.h>
#include "wasm_export.h"
#include "wasm_runtime.h"

#define WASM_STACK_SIZE 8192
#define WASM_HEAP_SIZE 8192
#define SENSOR_SAMPLE_INTERVAL_MS 1000

static char wasm_runtime_buffer[WASM_STACK_SIZE + WASM_HEAP_SIZE];
static const struct device *const sensor_dev = DEVICE_DT_GET_ONE(st_lsm6dsl);

// WASM module globals
static wasm_module_t wasm_module = NULL;
static wasm_module_inst_t wasm_module_inst = NULL;
static wasm_exec_env_t exec_env = NULL;

struct sensor_data {
    double temperature;
    double humidity;
    double pressure;
    double accel[3];
};

// Function type definitions for WASM exports
typedef void (*wasm_constructor_f)(wasm_exec_env_t exec_env, float temp, float humid, 
                                 float press, float accel_x, float accel_y, float accel_z);
typedef char* (*process_data_f)(wasm_exec_env_t exec_env, void* data);

static void read_sensor_data(struct sensor_data *data) {
    struct sensor_value temp, hum, press;
    struct sensor_value accel[3];

    if (!device_is_ready(sensor_dev)) {
        printk("Error: Device %s is not ready\n", sensor_dev->name);
        return;
    }

    // Read temperature
    sensor_sample_fetch_chan(sensor_dev, SENSOR_CHAN_AMBIENT_TEMP);
    sensor_channel_get(sensor_dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
    data->temperature = sensor_value_to_double(&temp);

    // Read humidity
    sensor_sample_fetch_chan(sensor_dev, SENSOR_CHAN_HUMIDITY);
    sensor_channel_get(sensor_dev, SENSOR_CHAN_HUMIDITY, &hum);
    data->humidity = sensor_value_to_double(&hum);

    // Read pressure
    sensor_sample_fetch_chan(sensor_dev, SENSOR_CHAN_PRESS);
    sensor_channel_get(sensor_dev, SENSOR_CHAN_PRESS, &press);
    data->pressure = sensor_value_to_double(&press);

    // Read accelerometer
    sensor_sample_fetch_chan(sensor_dev, SENSOR_CHAN_ACCEL_XYZ);
    sensor_channel_get(sensor_dev, SENSOR_CHAN_ACCEL_X, &accel[0]);
    sensor_channel_get(sensor_dev, SENSOR_CHAN_ACCEL_Y, &accel[1]);
    sensor_channel_get(sensor_dev, SENSOR_CHAN_ACCEL_Z, &accel[2]);
    
    for (int i = 0; i < 3; i++) {
        data->accel[i] = sensor_value_to_double(&accel[i]);
    }
}

static bool init_wasm_module(void) {
    // Initialize WAMR
    RuntimeInitArgs init_args;
    memset(&init_args, 0, sizeof(RuntimeInitArgs));
    init_args.mem_alloc_type = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf = wasm_runtime_buffer;
    init_args.mem_alloc_option.pool.heap_size = sizeof(wasm_runtime_buffer);

    if (!wasm_runtime_full_init(&init_args)) {
        printk("Error: failed to initialize WAMR runtime\n");
        return false;
    }

    // Load WASM module
    uint8_t *wasm_file_buf = NULL;
    uint32_t wasm_file_size;
    // TODO: Load WASM binary from file system
    
    // Instantiate module
    wasm_module = wasm_runtime_load(wasm_file_buf, wasm_file_size, NULL, 0);
    if (!wasm_module) {
        printk("Error: failed to load WASM module\n");
        return false;
    }

    wasm_module_inst = wasm_runtime_instantiate(wasm_module, WASM_STACK_SIZE, 
                                              WASM_HEAP_SIZE, NULL, 0);
    if (!wasm_module_inst) {
        printk("Error: failed to instantiate WASM module\n");
        return false;
    }

    exec_env = wasm_runtime_create_exec_env(wasm_module_inst, WASM_STACK_SIZE);
    if (!exec_env) {
        printk("Error: failed to create exec env\n");
        return false;
    }

    return true;
}

void main(void)
{
    // Initialize WASM runtime and module
    if (!init_wasm_module()) {
        return;
    }

    // Initialize sensor
    if (!device_is_ready(sensor_dev)) {
        printk("Error: Sensor device not ready\n");
        return;
    }

    struct sensor_data data;
    wasm_constructor_f constructor = wasm_runtime_lookup_function(wasm_module_inst, "SensorData.new", NULL);
    process_data_f process_data = wasm_runtime_lookup_function(wasm_module_inst, "process_sensor_data", NULL);

    if (!constructor || !process_data) {
        printk("Error: Failed to find WASM functions\n");
        return;
    }

    // Main loop
    while (1) {
        // Read sensor data
        read_sensor_data(&data);

        // Create SensorData object in WASM
        constructor(exec_env, (float)data.temperature, (float)data.humidity,
                   (float)data.pressure, (float)data.accel[0],
                   (float)data.accel[1], (float)data.accel[2]);

        // Process data
        char* result = process_data(exec_env, NULL);
        if (result) {
            printk("Processed data: %s\n", result);
            wasm_runtime_free(result);
        }

        printk("Temp: %.2f°C, Humidity: %.2f%%, Pressure: %.2fhPa\n",
               data.temperature, data.humidity, data.pressure);
        printk("Accel: X=%.2f, Y=%.2f, Z=%.2f\n",
               data.accel[0], data.accel[1], data.accel[2]);

        k_sleep(K_MSEC(SENSOR_SAMPLE_INTERVAL_MS));
    }

    // Cleanup
    wasm_runtime_destroy_exec_env(exec_env);
    wasm_runtime_deinstantiate(wasm_module_inst);
    wasm_runtime_unload(wasm_module);
    wasm_runtime_destroy();
}
