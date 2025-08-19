# ESP32S3 Xiao board
west build -b xiao_esp32s3/esp32s3/procpu -p auto -d build/xiaoS3 -- \
  -DCONF_FILE="prj.conf;prj_xiao_esp32s3.conf" \
  -DDTC_OVERLAY_FILE=boards/xiao_esp32s3_esp32s3_procpu.overlay


# ESP32C6 Xiao board
west build -b xiao_esp32c6/esp32c6/hpcore -p auto -d build/xiaoC6 -- \
  -DCONF_FILE="prj.conf;prj_xiao_esp32c6.conf" \
  -DDTC_OVERLAY_FILE=boards/xiao_esp32c6_esp32c6_hpcore.overlay

# RP2040 Xiao board
west build -b xiao_rp2040/rp2040 -p auto -d build/xiao2040 -- \
  -DCONF_FILE="prj.conf;prj_xiao_rp2040.conf" \
  -DDTC_OVERLAY_FILE=boards/xiao_rp2040_rp2040.overlay

# nRF52840 Xiao board
west build -b xiao_ble/nrf52840 -p auto -d build/xiao52 -- \
  -DCONF_FILE="prj.conf;prj_xiao_nrf52840.conf" \
  -DDTC_OVERLAY_FILE=boards/xiao_ble_nrf52840.overlay

# nRF54L15 Xiao board
west build -b xiao_nrf54l15/nrf54l15/cpuapp -p auto -d build/xiao54 -- \
  -DCONF_FILE="prj.conf;prj_xiao_nrf54l15.conf" \
  -DDTC_OVERLAY_FILE=boards/xiao_nrf54l15_nrf54l15_cpuapp.overlay


