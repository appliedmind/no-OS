/***************************************************************************//**
 *   @file   linux/linux_gpio.c
 *   @brief  Implementation of Linux platform GPIO Driver.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "error.h"
#include "gpio.h"
#include "delay.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct linux_gpio_desc
 * @brief Linux platform specific GPIO descriptor
 */
struct linux_gpio_desc {
	/** /sys/class/gpio/gpio"number"/direction file descriptor */
	int direction_fd;
	/** /sys/class/gpio/gpio"number"/value file descriptor */
	int value_fd;
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

#define GPIO_TIMEOUT_MS 1000

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO initialization parameters
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t linux_gpio_get(struct gpio_desc **desc,
		       const struct gpio_init_param *param)
{
	struct linux_gpio_desc *linux_desc;
	struct gpio_desc *descriptor;
	char path[64];
	int fd;
	int len;
	int ret;
	int timeout;

	descriptor = calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return FAILURE;

	linux_desc = calloc(1, sizeof(*linux_desc));
	if (!linux_desc)
		goto free_desc;

	descriptor->extra = linux_desc;
	descriptor->number = param->number;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (fd < 0) {
		printf("%s: Can't open device\n\r", __func__);
		goto free_linux_desc;
	}

	len = sprintf(path, "%d", descriptor->number);
	ret = write(fd, path, len);
	if (ret < 0) {
		printf("%s: Can't write to file\n\r", __func__);
		close(fd);
		goto free_linux_desc;
	}

	ret = close(fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		goto free_linux_desc;
	}

	sprintf(path, "/sys/class/gpio/gpio%d/direction", descriptor->number);
	timeout = GPIO_TIMEOUT_MS;
	while (--timeout) {
		linux_desc->direction_fd = open(path, O_WRONLY);
		if (linux_desc->direction_fd >= 0)
			break;
		mdelay(1);
	}
	if (linux_desc->direction_fd < 0) {
		printf("%s: Can't open %s\n\r", __func__, path);
		goto free_linux_desc;
	}

	sprintf(path, "/sys/class/gpio/gpio%d/value", descriptor->number);
	timeout = GPIO_TIMEOUT_MS;
	while (--timeout) {
		linux_desc->value_fd = open(path, O_WRONLY);
		if (linux_desc->value_fd >= 0)
			break;
		mdelay(1);
	}
	if (linux_desc->value_fd < 0) {
		printf("%s: Can't open %s\n\r", __func__, path);
		goto close_dir;
	}

	*desc = descriptor;

	return SUCCESS;

close_dir:
	close(linux_desc->direction_fd);
free_linux_desc:
	free(linux_desc);
free_desc:
	free(descriptor);

	return FAILURE;
}

/**
 * @brief Get the value of an optional GPIO.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t linux_gpio_get_optional(struct gpio_desc **desc,
				const struct gpio_init_param *param)
{
	linux_gpio_get(desc, param);

	return SUCCESS;
}

/**
 * @brief Free the resources allocated by gpio_get().
 * @param desc - The SPI descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t linux_gpio_remove(struct gpio_desc *desc)
{
	struct linux_gpio_desc *linux_desc;
	char path[64];
	int fd;
	int len;
	int ret;

	linux_desc = desc->extra;

	ret = close(linux_desc->direction_fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		return FAILURE;
	}

	ret = close(linux_desc->value_fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		return FAILURE;
	}

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd < 0) {
		printf("%s: Can't open device\n\r", __func__);
		return FAILURE;
	}

	len = sprintf(path, "%d", desc->number);
	ret = write(fd, path, len);
	if (ret < 0) {
		printf("%s: Can't write to file\n\r", __func__);
		return FAILURE;
	}

	ret = close(fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		return FAILURE;
	}

	free(desc->extra);
	free(desc);

	return SUCCESS;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t linux_gpio_set_value(struct gpio_desc *desc,
			     uint8_t value)
{
	struct linux_gpio_desc *linux_desc;
	int ret;

	linux_desc = desc->extra;

	if (value)
		ret = write(linux_desc->value_fd, "1", 2);
	else
		ret = write(linux_desc->value_fd, "0", 2);
	if (ret < 0) {
		printf("%s: Can't write to file\n\r", __func__);
		return FAILURE;
	}

	return SUCCESS;
}

/**
 * @brief Get the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t linux_gpio_get_value(struct gpio_desc *desc,
			     uint8_t *value)
{
	struct linux_gpio_desc *linux_desc;
	char data;
	int ret;

	linux_desc = desc->extra;

	ret = read(linux_desc->value_fd, &data, 1);
	if (ret < 0) {
		printf("%s: Can't read from file\n\r", __func__);
		return FAILURE;
	}

	if(data == '0')
		*value = GPIO_LOW;
	else
		*value = GPIO_HIGH;

	return SUCCESS;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t linux_gpio_direction_input(struct gpio_desc *desc)
{
	struct linux_gpio_desc *linux_desc;
	int ret;

	linux_desc = desc->extra;

	ret = write(linux_desc->direction_fd, "in", 3);
	if (ret < 0) {
		printf("%s: Can't write to file\n\r", __func__);
		return FAILURE;
	}

	return SUCCESS;
}

/**
 * @brief Enable the output direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t linux_gpio_direction_output(struct gpio_desc *desc,
				    uint8_t value)
{
	struct linux_gpio_desc *linux_desc;
	int ret;

	linux_desc = desc->extra;

	ret = write(linux_desc->direction_fd, "out", 4);
	if (ret < 0) {
		printf("%s: Can't write to file\n\r", __func__);
		return FAILURE;
	}

	ret = linux_gpio_set_value(desc, value);
	if (ret != SUCCESS) {
		printf("%s: Can't set value\n\r", __func__);
		return FAILURE;
	}

	return SUCCESS;
}

/**
 * @brief Get the direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param direction - The direction.
 *                    Example: GPIO_OUT
 *                             GPIO_IN
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t linux_gpio_get_direction(struct gpio_desc *desc,
				 uint8_t *direction)
{
	struct linux_gpio_desc *linux_desc;
	char data;
	int ret;

	linux_desc = desc->extra;

	ret = read(linux_desc->direction_fd, &data, 1);
	if (ret < 0) {
		printf("%s: Can't read from file\n\r", __func__);
		return FAILURE;
	}

	if (data == 'o')
		*direction = GPIO_OUT;
	else
		*direction = GPIO_IN;

	return SUCCESS;
}

/**
 * @brief Linux platform specific GPIO platform ops structure
 */
const struct gpio_platform_ops linux_gpio_ops = {
	.gpio_ops_get = &linux_gpio_get,
	.gpio_ops_get_optional = &linux_gpio_get_optional,
	.gpio_ops_remove = &linux_gpio_remove,
	.gpio_ops_direction_input = &linux_gpio_direction_input,
	.gpio_ops_direction_output = &linux_gpio_direction_output,
	.gpio_ops_get_direction = &linux_gpio_get_direction,
	.gpio_ops_set_value = &linux_gpio_set_value,
	.gpio_ops_get_value = &linux_gpio_get_value,
};
