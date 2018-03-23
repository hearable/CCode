#******************************************************************************
#
# Makefile - Rules for building the libraries, examples and docs.
#
# Copyright (c) 2017, Ambiq Micro
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
# 
# 3. Neither the name of the copyright holder nor the names of its
# contributors may be used to endorse or promote products derived from this
# software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# This is part of revision 1.2.11 of the AmbiqSuite Development Package.
#
#******************************************************************************
TARGET := multi_boot
COMPILERNAME := gcc
PROJECT := multi_boot_gcc
CONFIG := bin
AM_SoftwareRoot ?= ../../../../..

SHELL:=/bin/bash
#### Setup ####

STARTUP_FILE := ./startup_$(COMPILERNAME)

#### Required Executables ####
ECLIPSE := '$(shell which TrueSTUDIO 2>/dev/null)'
RM := $(shell which rm 2>/dev/null)

EXECUTABLES = ECLIPSE
K := $(foreach exec,$(EXECUTABLES),\
        $(if $(shell which $($(exec)) 2>/dev/null),,\
        $(info $(exec) not found on PATH ($($(exec))).)$(exec)))
$(if $(strip $(value K)),$(info Required Program(s) $(strip $(value K)) not found))

ifneq ($(strip $(value K)),)
all clean:
	$(info Tools $(COMPILERNAME) not installed.)
	$(RM) -rf bin
else

LIBS = ${AM_SoftwareRoot}/boards/apollo2_evb/bsp/atollic_gcc/bin/libam_bsp_gcc.a
LIBS += ${AM_SoftwareRoot}/mcu/apollo2/hal/atollic_gcc/bin/libam_hal_gcc.a
INCS = ${AM_SoftwareRoot}/boards/apollo2_evb/bsp/am_bsp_gpio.h
INCS += ${AM_SoftwareRoot}/mcu/apollo2/hal/am_hal_pin.h

#### Rules ####
all: directories $(CONFIG)/$(PROJECT).bin

# Source Dependencies must be defined before they are used.
SRCS = ../src/multi_boot.c
SRCS += ../src/multi_boot_secure.c
SRCS += ../../../../../utils/am_util_delay.c
SRCS += ../../../../../utils/am_util_stdio.c
SRCS += ../../../../../bootloader/am_bootloader.c
SRCS += ../../../../../bootloader/am_ios_boot_handlers.c
SRCS += ../../../../../bootloader/am_multi_boot.c
SRCS += ../../../../../bootloader/am_uart_boot_handlers.c

directories: $(CONFIG)

$(CONFIG):
	@mkdir -p $@

# BIN target requires post build step: objcopy elf to bin.
$(CONFIG)/$(PROJECT).bin: $(INCS) $(LIBS) $(SRCS)
	@echo " Building $(COMPILERNAME) $@..." ;\
	WKSP=$$(mktemp -d) ;\
	$(ECLIPSE) --launcher.suppressErrors -nosplash -data "$$WKSP" \
	    -application org.eclipse.cdt.managedbuilder.core.headlessbuild \
	    -import ./  -E cross_rm=$(RM) -cleanBuild .*/.*$(CONFIG).* \
	     1> '$(PROJECT).log' 2> '$(PROJECT).errlog' && $(RM) -rf $$WKSP

OBJS =  $(CONFIG)/**/* \
	$(STARTUP_FILE).*


clean:
	@echo "Cleaning..." ;\
	$(RM) -f $(OBJS) \
	    $(CONFIG)/$(PROJECT).bin  $(CONFIG)/$(PROJECT).elf \
	    $(CONFIG)/$(PROJECT).list $(CONFIG)/$(PROJECT).map \
	    $(PROJECT).log  $(PROJECT).errlog
endif
.PHONY: all clean directories

