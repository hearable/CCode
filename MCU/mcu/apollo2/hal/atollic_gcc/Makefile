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
TARGET := libam_hal
COMPILERNAME := gcc
PROJECT := libam_hal_gcc
CONFIG := bin
AM_SoftwareRoot ?= ../../../..

SHELL:=/bin/bash
#### Setup ####

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

LIBS = 
INCS = ${AM_SoftwareRoot}/mcu/apollo2/hal/am_hal_pin.h

#### Rules ####
all: directories $(CONFIG)/$(PROJECT).a

# Source Dependencies must be defined before they are used.
SRCS = .././am_hal_adc.c
SRCS += .././am_hal_cachectrl.c
SRCS += .././am_hal_clkgen.c
SRCS += .././am_hal_ctimer.c
SRCS += .././am_hal_debug.c
SRCS += .././am_hal_flash.c
SRCS += .././am_hal_global.c
SRCS += .././am_hal_gpio.c
SRCS += .././am_hal_i2c_bit_bang.c
SRCS += .././am_hal_interrupt.c
SRCS += .././am_hal_iom.c
SRCS += .././am_hal_ios.c
SRCS += .././am_hal_itm.c
SRCS += .././am_hal_mcuctrl.c
SRCS += .././am_hal_otp.c
SRCS += .././am_hal_pdm.c
SRCS += .././am_hal_pwrctrl.c
SRCS += .././am_hal_queue.c
SRCS += .././am_hal_reset.c
SRCS += .././am_hal_rtc.c
SRCS += .././am_hal_stimer.c
SRCS += .././am_hal_sysctrl.c
SRCS += .././am_hal_systick.c
SRCS += .././am_hal_tpiu.c
SRCS += .././am_hal_ttp.c
SRCS += .././am_hal_uart.c
SRCS += .././am_hal_vcomp.c
SRCS += .././am_hal_wdt.c

directories: $(CONFIG)

$(CONFIG):
	@mkdir -p $@

# BIN target requires post build step: objcopy elf to bin.
$(CONFIG)/$(PROJECT).a: $(INCS) $(LIBS) $(SRCS)
	@echo " Building $(COMPILERNAME) $@..." ;\
	WKSP=$$(mktemp -d) ;\
	$(ECLIPSE) --launcher.suppressErrors -nosplash -data "$$WKSP" \
	    -application org.eclipse.cdt.managedbuilder.core.headlessbuild \
	    -import ./  -E cross_rm=$(RM) -cleanBuild .*/.*$(CONFIG).* \
	     1> '$(PROJECT).log' 2> '$(PROJECT).errlog' && $(RM) -rf $$WKSP

OBJS =  $(CONFIG)/**/*


clean:
	@echo "Cleaning..." ;\
	$(RM) -rf $(CONFIG)
endif
.PHONY: all clean directories

