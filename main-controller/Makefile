#### VARIABLES ####
SHELL := /bin/bash
BOARD := esp32:esp32:esp32
PORT  := /dev/cu.SLAB_USBtoUART

# esp32.ttgo-lora32-v1


.PHONY: list
list: 
	arduino-cli board list

.PHONY: compile
compile:
	arduino-cli compile --fqbn $(BOARD) ../main-controller

.PHONY: write
write:
	arduino-cli upload -p $(PORT) --fqbn $(BOARD) ../main-controller