# Root build directory
BUILD_DIR ?= build

.PHONY: help clean \
        build-xiaoS3 flash-xiaoS3 \
        build-xiaoC6 flash-xiaoC6 \
        build-xiao2040 flash-xiao2040 \
        build-xiao52 flash-xiao52 \
        build-xiao54 flash-xiao54

help:
	@echo "Available targets:"
	@echo "  make build-xiaoS3     - build for Seeed Studio XIAO ESP32S3"
	@echo "  make flash-xiaoS3     - flash to ESP32S3"
	@echo "  make build-xiaoC6     - build for Seeed Studio XIAO ESP32C6"
	@echo "  make flash-xiaoC6     - flash to ESP32C6"
	@echo "  make build-xiao2040   - build for Seeed Studio XIAO RP2040"
	@echo "  make flash-xiao2040   - flash to RP2040"
	@echo "  make build-xiao52     - build for Seeed Studio XIAO nRF52840"
	@echo "  make flash-xiao52     - flash to nRF52840 (UF2)"
	@echo "  make build-xiao54     - build for Seeed Studio XIAO nRF54L15"
	@echo "  make flash-xiao54     - flash to nRF54L15"
	@echo "  make clean            - remove all build directories"

# ----------------------------
# ESP32S3 Xiao
# ----------------------------
build-xiaoS3:
	west build -b xiao_esp32s3/esp32s3/procpu -p always -d $(BUILD_DIR)/xiaoS3 -- \
		-DCONF_FILE="prj.conf;prj_xiao_esp32s3.conf" \
		-DDTC_OVERLAY_FILE=boards/xiao_esp32s3_esp32s3_procpu.overlay

flash-xiaoS3:
	west flash -d $(BUILD_DIR)/xiaoS3

# ----------------------------
# ESP32C6 Xiao
# ----------------------------
build-xiaoC6:
	west build -b xiao_esp32c6/esp32c6/hpcore -p always -d $(BUILD_DIR)/xiaoC6 -- \
		-DCONF_FILE="prj.conf;prj_xiao_esp32c6.conf" \
		-DDTC_OVERLAY_FILE=boards/xiao_esp32c6_esp32c6_hpcore.overlay

flash-xiaoC6:
	west flash -d $(BUILD_DIR)/xiaoC6

# ----------------------------
# RP2040 Xiao
# ----------------------------
build-xiao2040:
	west build -b xiao_rp2040/rp2040 -p always -d $(BUILD_DIR)/xiao2040 -- \
		-DCONF_FILE="prj.conf;prj_xiao_rp2040.conf" \
		-DDTC_OVERLAY_FILE=boards/xiao_rp2040_rp2040.overlay

flash-xiao2040:
	west flash -d $(BUILD_DIR)/xiao2040

# ----------------------------
# nRF52840 Xiao
# ----------------------------
build-xiao52:
	west build -b xiao_ble/nrf52840 -p always -d $(BUILD_DIR)/xiao52 -- \
		-DCONF_FILE="prj.conf;prj_xiao_nrf52840.conf" \
		-DDTC_OVERLAY_FILE=boards/xiao_ble_nrf52840.overlay

flash-xiao52:
	west flash -d $(BUILD_DIR)/xiao52 -r uf2

# ----------------------------
# nRF54L15 Xiao
# ----------------------------
build-xiao54:
	west build -b xiao_nrf54l15/nrf54l15/cpuapp -p always -d $(BUILD_DIR)/xiao54 -- \
		-DCONF_FILE="prj.conf;prj_xiao_nrf54l15.conf" \
		-DDTC_OVERLAY_FILE=boards/xiao_nrf54l15_nrf54l15_cpuapp.overlay

flash-xiao54:
	west flash -d $(BUILD_DIR)/xiao54

# ----------------------------
# Cleanup
# ----------------------------
clean:
	rm -rf $(BUILD_DIR)
