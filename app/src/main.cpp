#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(multiSensor, CONFIG_MESH_LOG_LEVEL);

#include "sensors.h"
// Allow logging, with default of warn and errors allowed





int main(void){
	
    int num_sensors, num_reads;

    num_sensors = sensor_init();


    if (num_sensors > 0) {
        while(1) {
            num_reads = sensor_read();
            if(num_reads < num_sensors) LOG_WRN("At least one sensor did not read");
            k_sleep(K_SECONDS(5));
        }
    }
    else {
        while(1){
            LOG_WRN("No sensors initialized. Exiting");
            k_sleep(K_SECONDS(5));
        }
    }

	return 0;
}