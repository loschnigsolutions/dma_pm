/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "Timer.h"
#include "BlinkLed.h"

// ----------------------------------------------------------------------------
//
// Standalone STM32F4 led blink sample (trace via DEBUG).
//
// In debug configurations, demonstrate how to print a greeting message
// on the trace device. In release configurations the message is
// simply discarded.
//
// Then demonstrates how to blink a led with 1 Hz, using a
// continuous loop and SysTick delays.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- Timing definitions -------------------------------------------------

// Keep the LED on for 2/3 of a second.
#define BLINK_ON_TICKS  (TIMER_FREQUENCY_HZ * 3 / 4)
#define BLINK_OFF_TICKS (TIMER_FREQUENCY_HZ - BLINK_ON_TICKS)

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int
main(int argc, char* argv[])
{
	//uint8_t msg[34] = "Welcome to world of programming\r\n"

	volatile char msg[1024];

	USART_TypeDef *u6 = USART6;
	GPIO_TypeDef  *pc = GPIOC;

	DMA_Stream_TypeDef *d2 = DMA2_Stream1;	//Ch5_Stream1

	//Clocks

	RCC->AHB1ENR	|=	RCC_AHB1ENR_DMA2EN;
	RCC->AHB1ENR	|=	RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR	|=	RCC_APB2ENR_USART6EN;

	//GPIO Configuration

	pc->MODER	|=	GPIO_MODER_MODER6_1;
	pc->MODER	|=	GPIO_MODER_MODER7_1;

	pc->AFR[0]	|=  (8 << 24)|(8 << 28);

	//UART Configuration

	u6->BRR	= (84000000/9600);

	u6->CR1	|= USART_CR1_UE;
	u6->CR1	|= USART_CR1_RE;
	u6->CR3	|= USART_CR3_DMAR;

	//DMA Configuration

	d2->CR	|=	(5 << 25);	//Channel Selection
	d2->CR	|=	(1 << 10);	//MemInc Mode
	d2->CR	|=	(1 << 5);	//PFCTRL

	d2->PAR	  = &u6->DR;
	d2->M0AR  = msg;
	//d2->NDTR  = 1024;

	d2->CR	|=	(1 << 0);

	//Make Buffer Empty
	memset(msg,'\0',1024);

	while(1)
	{
		trace_printf("String: %s\n",msg);
	}


}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
