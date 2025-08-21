#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(multiSensor);
extern "C" {
    #include <zephyr/sys/printk.h>
}


/* ----------------------------------------------------------------
 * AHT20 sensor
 * ---------------------------------------------------------------- */
#if DT_NODE_EXISTS(DT_NODELABEL(aht20))
#define AHT_NODE DT_NODELABEL(aht20)

#define AHT_STATUS_ADDR            0x71
#define AHT_STATUS_ID_MASK         0x18
#define AHT_BUSY_MASK              0x80
#define AHT_DATA_LEN               6
#define AHT_TRIGGER_MEASURE_LEN    3

static const struct i2c_dt_spec aht_i2c = I2C_DT_SPEC_GET(AHT_NODE);

int aht20_read(const struct i2c_dt_spec *i2c_dev) {
    uint8_t trigger_measure[AHT_TRIGGER_MEASURE_LEN] = {0xAC, 0x33, 0x00};
    uint8_t status_reg = AHT_STATUS_ADDR;
    uint8_t id;
    uint8_t data[AHT_DATA_LEN] = {0};
    int ret;

    ret = i2c_write_dt(i2c_dev, trigger_measure, sizeof(trigger_measure));
    if (ret) {
        LOG_ERR("AHT20: failed to send measurement command");
        return ret;
    }

    k_msleep(80);

    ret = i2c_write_read_dt(i2c_dev, &status_reg, 1, &id, 1);
    if (ret) {
        LOG_ERR("AHT20: failed to read status register");
        return ret;
    }

    if (id & AHT_BUSY_MASK) {
        LOG_ERR("AHT20: still busy after 80ms");
        return -EBUSY;
    }

    ret = i2c_read_dt(i2c_dev, data, sizeof(data));
    if (ret) {
        LOG_ERR("AHT20: failed to read sensor data");
        return ret;
    }

    uint32_t raw_humidity = ((uint32_t)data[1] << 12) |
                            ((uint32_t)data[2] << 4) |
                            ((data[3] & 0xF0) >> 4);
    uint32_t raw_temperature = ((uint32_t)(data[3] & 0x0F) << 16) |
                               ((uint32_t)data[4] << 8) | data[5];

    float humidity = ((float)raw_humidity * 100.0f) / 1048576.0f;
    float temperature = ((float)raw_temperature * 200.0f) / 1048576.0f - 50.0f;

    LOG_INF("AHT20 Humidity: %.2f %%", (double)humidity);
    LOG_INF("AHT20 Temperature: %.2f °C", (double)temperature);
    LOG_DBG("AHT20 Raw data: %02X %02X %02X %02X %02X %02X",
            data[0], data[1], data[2], data[3], data[4], data[5]);

    return 0;
}

int aht20_init(const struct i2c_dt_spec *i2c_dev) {
    if (!device_is_ready(i2c_dev->bus)) {
        LOG_ERR("AHT20 I2C bus not ready");
        return -ENODEV;
    }

    uint8_t status_reg = AHT_STATUS_ADDR;
    uint8_t id;
    if (i2c_write_read_dt(i2c_dev, &status_reg, 1, &id, 1) ||
        (id & AHT_STATUS_ID_MASK) != AHT_STATUS_ID_MASK) {
        LOG_ERR("AHT20: invalid initial status (0x%02X)", id);
        return -EIO;
    }

    LOG_INF("AHT20 initialized");
    return 0;
}
#endif /* AHT20 */

/* ----------------------------------------------------------------
 * SGP30 sensor
 * ---------------------------------------------------------------- */
#if DT_NODE_EXISTS(DT_NODELABEL(sgp30))
#define SGP_NODE DT_NODELABEL(sgp30)

static const struct i2c_dt_spec sgp_i2c = I2C_DT_SPEC_GET(SGP_NODE);

static uint8_t sgp_crc8(uint8_t *data, size_t len) {
    uint8_t crc = 0xFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++) {
            crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : (crc << 1);
        }
    }
    return crc;
}

int sgp30_init(const struct i2c_dt_spec *i2c_dev) {
    uint8_t cmd[2] = {0x20, 0x03}; /* IAQ init command */
    int ret = i2c_write_dt(i2c_dev, cmd, sizeof(cmd));
    if (ret) {
        LOG_ERR("SGP30: failed to send init command");
        return ret;
    }
    k_msleep(10);
    LOG_INF("SGP30 initialized");
    return 0;
}

int sgp30_read(const struct i2c_dt_spec *i2c_dev) {
    uint8_t cmd[2] = {0x20, 0x08}; /* measure air quality */
    uint8_t data[6];
    int ret = i2c_write_dt(i2c_dev, cmd, sizeof(cmd));
    if (ret) {
        LOG_ERR("SGP30: failed to send measure command");
        return ret;
    }

    k_msleep(12);

    ret = i2c_read_dt(i2c_dev, data, sizeof(data));
    if (ret) {
        LOG_ERR("SGP30: failed to read data");
        return ret;
    }

    if (sgp_crc8(data, 2) != data[2] || sgp_crc8(data + 3, 2) != data[5]) {
        LOG_ERR("SGP30: CRC error");
        return -EIO;
    }

    uint16_t eCO2 = (data[0] << 8) | data[1];
    uint16_t TVOC = (data[3] << 8) | data[4];

    LOG_INF("SGP30 eCO2: %u ppm, TVOC: %u ppb", eCO2, TVOC);
    return 0;
}
#endif /* SGP30 */

/* ----------------------------------------------------------------
 * Unified init/read wrappers
 * ---------------------------------------------------------------- */
int sensor_init(void) {
    int ret = 0;

#if DT_NODE_EXISTS(DT_NODELABEL(sgp30))
    ret += 1 - sgp30_init(&sgp_i2c);
    // FIXME replace with logic in event of failure to init
#endif
#if DT_NODE_EXISTS(DT_NODELABEL(aht20))
    ret += 1 - aht20_init(&aht_i2c);
    // FIXME replace with logic in event of failure to init
#endif
    return ret;
}

int sensor_read(void) {
    int ret = 0;

#if DT_NODE_EXISTS(DT_NODELABEL(sgp30))
    ret += 1 - sgp30_read(&sgp_i2c);
#endif
#if DT_NODE_EXISTS(DT_NODELABEL(aht20))
    ret += 1 - aht20_read(&aht_i2c);
#endif
    return ret;
}
