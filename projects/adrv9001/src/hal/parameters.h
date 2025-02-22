/***************************************************************************//**
 *   @file   adrv9001/src/hal/parameters.h
 *   @brief  Platform dependent parameters.
 *   @author Darius Berghe (darius.berghe@analog.com)
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
#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "xparameters.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define ADRV9001_NUM_SUBCHANNELS	2 /* I - in-phase and Q - quadrature channels */
#define ADRV9001_NUM_CHAIN_CHANNELS	2 /* channels per RX/TX chain */
#define ADRV9001_NUM_CHANNELS		(ADRV9001_NUM_CHAIN_CHANNELS * ADRV9001_NUM_SUBCHANNELS)

#define DAC_BUFFER_SAMPLES 1024
#define ADC_BUFFER_SAMPLES 16384
#define ADC1_CHANNELS 4
#define ADC2_CHANNELS 2

#ifdef XPS_BOARD_ZCU102
#define GPIO_OFFSET			78
#else
#define GPIO_OFFSET			54
#endif

/* GPIO */
#define GPIO_RESET			(46 + GPIO_OFFSET)
#define GPIO_SSI_SYNC			(54 + GPIO_OFFSET)

/* SPI */
#define SPI_CS				0

#ifdef XPS_BOARD_ZCU102
#define SPI_DEVICE_ID			XPAR_PSU_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID			XPAR_PSU_GPIO_0_DEVICE_ID
#else
#define SPI_DEVICE_ID			XPAR_PS7_SPI_0_DEVICE_ID
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#endif

/* UART */
#define UART_DEVICE_ID			XPAR_XUARTPS_0_DEVICE_ID
#ifdef XPS_BOARD_ZCU102
#define UART_IRQ_ID			XPAR_XUARTPS_0_INTR
#else
#define UART_IRQ_ID			XPAR_XUARTPS_1_INTR
#endif

/* Interrupt */
#define INTC_DEVICE_ID			XPAR_SCUGIC_SINGLE_DEVICE_ID
#endif

/* DMA Controller */
#define RX1_DMA_BASEADDR		XPAR_AXI_ADRV9001_RX1_DMA_BASEADDR
#define RX2_DMA_BASEADDR		XPAR_AXI_ADRV9001_RX2_DMA_BASEADDR
#define TX1_DMA_BASEADDR		XPAR_AXI_ADRV9001_TX1_DMA_BASEADDR
#define TX2_DMA_BASEADDR		XPAR_AXI_ADRV9001_TX2_DMA_BASEADDR

/* AXI ADC/DAC */
#define RX1_ADC_BASEADDR		XPAR_AXI_ADRV9001_BASEADDR
#define RX2_ADC_BASEADDR		(XPAR_AXI_ADRV9001_BASEADDR + 0x1000)
#define TX1_DAC_BASEADDR		(XPAR_AXI_ADRV9001_BASEADDR + 0x2000)
#define TX2_DAC_BASEADDR		(XPAR_AXI_ADRV9001_BASEADDR + 0x4000)

/* ADC/DAC Buffers */
static uint32_t dac1_buffer[DAC_BUFFER_SAMPLES] __attribute__((aligned));
static uint16_t adc1_buffer[ADC_BUFFER_SAMPLES*ADC1_CHANNELS] __attribute__((
			aligned));
#ifndef ADRV9002_RX2TX2
static uint32_t dac2_buffer[DAC_BUFFER_SAMPLES] __attribute__((aligned));
static uint16_t adc2_buffer[ADC_BUFFER_SAMPLES*ADC2_CHANNELS] __attribute__((
			aligned));
#endif
