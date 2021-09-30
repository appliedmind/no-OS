################################################################################
#									       #
#     Shared variables:							       #
#	- PROJECT							       #
#	- DRIVERS							       #
#	- INCLUDE							       #
#	- PLATFORM_DRIVERS						       #
#	- NO-OS								       #
#									       #
################################################################################

# Add to SRCS source files to be build in the project
SRCS += $(PROJECT)/src/ad7768_evb.c
SRCS += $(NO-OS)/util/fifo.c
SRCS += $(NO-OS)/util/util.c
SRCS += $(NO-OS)/util/list.c

# Add to INCS inlcude files to be build in the project
INCS += $(INCLUDE)/error.h
INCS += $(INCLUDE)/gpio.h
INCS += $(INCLUDE)/delay.h
INCS += $(INCLUDE)/util.h
INCS += $(INCLUDE)/axi_io.h
INCS += $(INCLUDE)/spi.h
INCS += $(INCLUDE)/timer.h
INCS += $(INCLUDE)/i2c.h
INCS += $(INCLUDE)/uart.h
INCS +=	$(INCLUDE)/irq.h
INCS += $(INCLUDE)/list.h
INCS += $(INCLUDE)/fifo.h
INCS += $(PROJECT)/src/parameters.h

# Add to SRC_DIRS directories to be used in the build. All .c and .h files from
# the directory and subdirectories will be added to the build (recursively)
SRC_DIRS += $(PLATFORM_DRIVERS)
SRC_DIRS += $(DRIVERS)/adc/ad7768
SRC_DIRS += $(DRIVERS)/axi_core/axi_dmac
SRC_DIRS += $(DRIVERS)/axi_core/axi_adc_core
SRC_DIRS += $(DRIVERS)/spi
SRC_DIRS += $(DRIVERS)/i2c
SRC_DIRS += $(DRIVERS)/gpio
SRC_DIRS += $(DRIVERS)/irq

# Add to LIBRARIES the libraries that need to be linked in the build
# LIBRARIES += mqtt
# LIBRARIES += fatfs
# LIBRARIES += mbedtls
# LIBRARIES += iio

ifeq (y,$(strip $(TINYIIOD)))
LIBRARIES += iio
SRC_DIRS += $(NO-OS)/iio/iio_app

INCS +=	$(INCLUDE)/fifo.h						\
	$(INCLUDE)/uart.h						\
	$(INCLUDE)/list.h						\
	$(PLATFORM_DRIVERS)/irq_extra.h					\
	$(PLATFORM_DRIVERS)/uart_extra.h                                \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.h
SRCS += $(NO-OS)/util/fifo.c						\
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.c			\
	$(NO-OS)/util/list.c						\
	$(PLATFORM_DRIVERS)/uart.c					\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c
endif
