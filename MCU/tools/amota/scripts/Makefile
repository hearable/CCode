#*******************************************************************************
#
# Simple Makefile to prepare binaries for AMOTA.
#
#*******************************************************************************

FLAG_ADDR_APOLLO1 = 0x6000
LOAD_ADDR_APOLLO1 = 0x8000
APPBIN_APOLLO1_PROD =../../../boards/apollo1_evb_am_ble/examples/freertos_amota/keil/bin/freertos_amota.bin
BOOTBIN_APOLLO1_PROD =../../../boards/apollo1_evb/examples/multi_boot/keil/bin/multi_boot.bin
UPDATEBIN_APOLLO1 =../../../boards/apollo1_evb_am_ble/examples/freertos_amota_blinky/keil/bin/freertos_amota_blinky.bin
FLAG_ADDR_APOLLO2 = 0x6000
LOAD_ADDR_APOLLO2 = 0x8000
APPBIN_APOLLO2_BLUE_PROD =../../../boards/apollo2_blue_evb/examples/freertos_amota/keil/bin/freertos_amota.bin
BOOTBIN_APOLLO2_BLUE_PROD =../../../boards/apollo2_evb/examples/multi_boot/keil/bin/multi_boot.bin
UPDATEBIN_APOLLO2_BLUE =../../../boards/apollo2_blue_evb/examples/freertos_amota_blinky/keil/bin/freertos_amota_blinky.bin

all: $(APPBIN_APOLLO1_PROD) $(BOOTBIN_APOLLO1_PROD) $(UPDATEBIN_APOLLO1) $(APPBIN_APOLLO2_BLUE_PROD) $(BOOTBIN_APOLLO2_BLUE_PROD) $(UPDATEBIN_APOLLO2_BLUE)
	python3 bootloader_binary_combiner.py --bootbin "${BOOTBIN_APOLLO1_PROD}" --appbin "${APPBIN_APOLLO1_PROD}" --flag-addr ${FLAG_ADDR_APOLLO1} --load-address ${LOAD_ADDR_APOLLO1} -o starter_binary_apollo1
	python3 ota_binary_converter.py --appbin "${UPDATEBIN_APOLLO1}" --load-address ${LOAD_ADDR_APOLLO1} -o update_binary_apollo1
	python3 bootloader_binary_combiner.py --bootbin "${BOOTBIN_APOLLO2_BLUE_PROD}" --appbin "${APPBIN_APOLLO2_BLUE_PROD}" --flag-addr ${FLAG_ADDR_APOLLO2} --load-address ${LOAD_ADDR_APOLLO2} -o starter_binary_apollo2_blue
	python3 ota_binary_converter.py --appbin "${UPDATEBIN_APOLLO2_BLUE}" --load-address ${LOAD_ADDR_APOLLO2} -o update_binary_apollo2_blue


$(APPBIN_APOLLO1_PROD):
	$(MAKE) -C ../../../boards/apollo1_evb_am_ble/examples/freertos_amota/keil/ $(MAKECMDGOALS)

$(BOOTBIN_APOLLO1_PROD):
	$(MAKE) -C ../../../boards/apollo1_evb/examples/multi_boot/keil/ $(MAKECMDGOALS)

$(UPDATEBIN_APOLLO1):
	$(MAKE) -C ../../../boards/apollo1_evb_am_ble/examples/freertos_amota_blinky/keil/ $(MAKECMDGOALS)

$(APPBIN_APOLLO2_BLUE_PROD):
	$(MAKE) -C ../../../boards/apollo2_blue_evb/examples/freertos_amota/keil/ $(MAKECMDGOALS)

$(BOOTBIN_APOLLO2_BLUE_PROD):
	$(MAKE) -C ../../../boards/apollo2_evb/examples/multi_boot/keil/ $(MAKECMDGOALS)

$(UPDATEBIN_APOLLO2_BLUE):
	$(MAKE) -C ../../../boards/apollo2_blue_evb/examples/freertos_amota_blinky/keil/ $(MAKECMDGOALS)



clean:
	rm -rf starter_binary_apollo1.bin starter_binary_apollo2_blue.bin update_binary_apollo1.bin update_binary_apollo2_blue.bin
	$(MAKE) -C ../../../boards/apollo1_evb/examples/multi_boot/keil/ $(MAKECMDGOALS)
	$(MAKE) -C ../../../boards/apollo1_evb_am_ble/examples/freertos_amota/keil/ $(MAKECMDGOALS)
	$(MAKE) -C ../../../boards/apollo1_evb_am_ble/examples/freertos_amota_blinky/keil/ $(MAKECMDGOALS)
	$(MAKE) -C ../../../boards/apollo2_evb/examples/multi_boot/keil/ $(MAKECMDGOALS)
	$(MAKE) -C ../../../boards/apollo2_blue_evb/examples/freertos_amota/keil/ $(MAKECMDGOALS)
	$(MAKE) -C ../../../boards/apollo2_blue_evb/examples/freertos_amota_blinky/keil/ $(MAKECMDGOALS)
	