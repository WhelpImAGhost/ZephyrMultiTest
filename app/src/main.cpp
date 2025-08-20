#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>


#define AHT_NODE DT_NODELABEL(aht20)
#define AHT_STATUS_ADDR	 0x71
#define AHT_STATUS_ID	 0x18


// Allow logging, with default of warn and errors allowed
LOG_MODULE_REGISTER(nrfMeshCore, CONFIG_MESH_LOG_LEVEL);



int aht20_read(i2c_dt_spec aht_i2c){

	  float humidity, temperature;
	  uint8_t trigger_measure[] = {0xAC, 0x33, 0x00};

	  int ret = 0;
	  uint8_t id = 0;
	  uint8_t regs[] = {AHT_STATUS_ADDR};
	  uint8_t data[6] = {0};

	ret = i2c_write_dt(&aht_i2c, trigger_measure, 3);

	if (ret != 0) {
		LOG_ERR("Failed to write TM command");
		return -1;
	}
	k_msleep(80);
    
	ret = i2c_write_read_dt(&aht_i2c, regs, 1, &id, 1);

	if (ret != 0) {
		LOG_ERR("Failed to get Status register");
		return -1;
	}

	if ((id & 0x80) == 0x80) {
		LOG_ERR("Detected still busy after 80ms");
		return -1;
	}

	LOG_DBG("Sensor ready for reading");

	
	ret = i2c_read_dt(&aht_i2c, data, 6);

	if (ret != 0) {
		LOG_ERR("Failed to get data");
		return -1;
	}

	LOG_INF("Data gathered:");

	  uint32_t raw_humidity = ((uint32_t)data[1] << 12) |
	((uint32_t)data[2] << 4) |
	((data[3] & 0xF0) >> 4);

	  uint32_t raw_temperature = ((uint32_t)(data[3] & 0x0F) << 16) |
		((uint32_t)data[4] << 8) |
		((uint32_t)data[5]);

	// Convert to actual values
	humidity = ((float)raw_humidity * 100.0f) / 1048576.0f;
	temperature = ((float)raw_temperature * 200.0f) / 1048576.0f - 50.0f;

	LOG_INF("Humidity: %.2f", (double)humidity);
	LOG_INF("Temperature: %.2f", (double)temperature);


	LOG_DBG("Raw data: %02X %02X %02X %02X %02X %02X",
		data[0], data[1], data[2], data[3], data[4], data[5]);
	return 0;

}



int main(void){
	
	uint8_t id = 0;
	uint8_t regs[1] = {AHT_STATUS_ADDR};
	int ret = 1;

	const struct i2c_dt_spec aht_i2c = I2C_DT_SPEC_GET(AHT_NODE);

	if (!device_is_ready(aht_i2c.bus)) {
		LOG_ERR("I2C bus %s is not ready!", aht_i2c.bus->name);
		return -1;
	}



	ret = i2c_write_read_dt(&aht_i2c, regs, 1, &id, 1);

	if (ret != 0) {
		LOG_ERR("Failed to read register %x", regs[0]);
		return -1;
	}

	if ((id & 0x18) != 0x18) {
		LOG_ERR("Invalid initial Status! %x", id);
		return -1;
	}


	while(1) {

		aht20_read(aht_i2c);

		k_sleep(K_SECONDS(5));

	}

	return 0;
}