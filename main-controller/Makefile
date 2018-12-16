#### VARIABLES ####
SHELL := /bin/bash
BOARD := arduino:samd:mkr1000
PORT  := /dev/cu.SLAB_USBtoUART


.PHONY: list
list: 
	arduino-cli board list

.PHONY: compile
compile:
	arduino-cli compile --fqbn $(BOARD) ../main-controller

.PHONY: write
write:
	arduino-cli upload -p $(PORT) --fqbn $(BOARD) ../main-controller