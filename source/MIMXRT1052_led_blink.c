/*
 * Copyright 2016-2022 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    MIMXRT1052_led_blink.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MIMXRT1052.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */
//#include "cr_section_macros.h"

/* TODO: insert other definitions and declarations here. */

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
extern uint32_t __Vectors[];
#define INT_VECTOR_ADDRESS ((uint32_t)__Vectors)
#elif defined(__MCUXPRESSO)
extern uint32_t __Vectors[];
#define INT_VECTOR_ADDRESS ((uint32_t)__Vectors)
extern unsigned int __section_table_start;
#elif defined(__ICCARM__)
extern uint32_t __VECTOR_TABLE[];
#define INT_VECTOR_ADDRESS ((uint32_t)__VECTOR_TABLE)
#elif defined(__GNUC__)
extern uint32_t __VECTOR_TABLE[];
#define INT_VECTOR_ADDRESS ((uint32_t)__VECTOR_TABLE)
#endif

__attribute__ ((section(".after_vectors.relocate_ivt")))
void SystemInitHook (void)
{
    unsigned int* int_vector_start = (void *)INT_VECTOR_ADDRESS;
    unsigned int* int_vector_end = &__section_table_start;
    unsigned int* sdram_ivt_start = (void *)0x20000000;

    while (int_vector_start < int_vector_end)
    {
        *sdram_ivt_start++ = *int_vector_start++;
    }

    SCB->VTOR = 0x20000000;
}

/*
 * @brief   Application entry point.
 */
int main(void) {

    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    NVIC_SetPriorityGrouping(4);
    SysTick_Config(SystemCoreClock / 1000);

    PRINTF("Hello World\n");

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
        /* 'Dummy' NOP to allow source level single stepping of
            tight while() loop */
        __asm volatile ("nop");

        uint32_t key = GPIO_PinRead(BOARD_INITPINS_SW3_PORT, BOARD_INITPINS_SW3_PIN);
        if (key == 0) {
            GPIO_PinWrite(BOARD_INITPINS_LED_PORT, BOARD_INITPINS_LED_PIN, 0);
        } else {
//            GPIO_PinWrite(BOARD_INITPINS_LED_PORT, BOARD_INITPINS_LED_PIN, 1);
        }
    }
    return 0 ;
}

void SysTick_Handler(void)
{
    static uint32_t timecnt = 0;
    ++timecnt;

    if (timecnt % 500 == 0)
    {
        GPIO_PortToggle(BOARD_INITPINS_LED_PORT, 1u << BOARD_INITPINS_LED_GPIO_PIN);
    }
}

void PIT_IRQHandler(void)
{
    while (1) {}
}
