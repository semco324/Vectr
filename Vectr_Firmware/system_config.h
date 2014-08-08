/*******************************************************************************
  MPLAB Harmony Demo Configuration Header

  Company:
    Microchip Technology Incorporated

  File Name:
    system_config.h

  Summary:
    Top-level configuration header file.

  Description:
    This file contains the configuration settings for the system. It contains
 * the constants for the I/O
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END

#ifndef _SYS_CONFIG_H
#define _SYS_CONFIG_H

#define PLIB_DISABLE_OPTIMIZATION_WARNING 1
#include "peripheral/peripheral.h"


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section Includes other configuration headers necessary to completely
    define this configuration.
*/

/*System Clock Frequency*/
#define SYS_CLOCK              96000000
#define PERIPHERAL_CLOCK       48000000


/*I2C Definitions*/
/*MGC3130 I2C Definitions*/
#define I2C_MGC3130                 I2C_ID_1
#define I2C_MGC3130_BAUD_RATE       400000
#define I2C_MGC3130_SLAVE_ADDRESS   0x1A

/*MGC3130 Port Definitions*/
#define MGC3130_RESET_PORT          PORT_CHANNEL_C
#define MGC3130_RESET_PIN           PORTS_BIT_POS_13
#define MGC3130_ACTIVATE_RESET      PLIB_PORTS_PinClear(PORTS_ID_0, MGC3130_RESET_PORT,MGC3130_RESET_PIN)
#define MGC3130_CLEAR_RESET         PLIB_PORTS_PinSet(PORTS_ID_0, MGC3130_RESET_PORT,MGC3130_RESET_PIN)

#define MGC3130_TS_PORT     PORT_CHANNEL_D
#define MGC3130_TS_PIN      PORTS_BIT_POS_0
#define MGC3130_TS_PIN_HIGH PLIB_PORTS_PinSet(PORTS_ID_0, MGC3130_TS_PORT,MGC3130_TS_PIN)
#define MGC3130_TS_PIN_LOW  PLIB_PORTS_PinClear(PORTS_ID_0, MGC3130_TS_PORT,MGC3130_TS_PIN)
#define TS_LINE_IS_LOW      !PLIB_PORTS_PinGet(PORTS_ID_0, MGC3130_TS_PORT,MGC3130_TS_PIN)

#define I2C_MGC3130_INT_SOURCE  INT_SOURCE_I2C_1_MASTER
#define ENABLE_I2C_MGC3130_INT  PLIB_INT_SourceEnable(INT_ID_0, I2C_MGC3130_INT_SOURCE)
#define DISABLE_I2C_MGC3130_INT  PLIB_INT_SourceDisable(INT_ID_0, I2C_MGC3130_INT_SOURCE)

#define MGC3130_DATA_READY_INT_SOURCE   INT_SOURCE_EXTERNAL_0
#define ENABLE_MGC3130_DATA_READY_INT   PLIB_INT_SourceEnable(INT_ID_0, MGC3130_DATA_READY_INT_SOURCE)
#define DISABLE_MGC3130_DATA_READY_INT   PLIB_INT_SourceDisable(INT_ID_0, MGC3130_DATA_READY_INT_SOURCE)
#define CLEAR_MGC3130_DATA_READY_INT    PLIB_INT_SourceFlagClear(INT_ID_0, MGC3130_DATA_READY_INT_SOURCE)


//Timers 4 and 5 combine to make a 32 bit timer
#define SAMPLE_TIMER    TMR_ID_4
#define SAMPLE_TIMER_PRESCALE   TMR_PRESCALE_VALUE_1
#define SAMPLE_TIMER_PERIOD     (PERIPHERAL_CLOCK/200)-1 //200Hz
#define SAMPLE_TIMER_INT_SOURCE INT_SOURCE_TIMER_5
#define ENABLE_SAMPLE_TIMER_INT  PLIB_INT_SourceEnable(INT_ID_0, SAMPLE_TIMER_INT_SOURCE)
#define DISABLE_SAMPLE_TIMER_INT PLIB_INT_SourceDisable(INT_ID_0, SAMPLE_TIMER_INT_SOURCE)
#define CLEAR_SAMPLE_TIMER_INT   PLIB_INT_SourceFlagClear(INT_ID_0, SAMPLE_TIMER_INT_SOURCE)
#define SET_SAMPLE_TIMER_PERIOD(newPeriod) PLIB_TMR_Period32BitSet(SAMPLE_TIMER, newPeriod)
#define RESET_SAMPLE_TIMER  PLIB_TMR_Counter32BitClear(SAMPLE_TIMER);

/*SPI Frequency*/
#define SPI_DAC             SPI_ID_1
#define SPI_DAC_BAUD_RATE   1600000 //1.6MHz - Max for the ADC.

/*SPI Port Definitions*/
#define DAC_SPI_EN_PORT PORT_CHANNEL_E
#define DAC_SPI_EN_PIN  PORTS_BIT_POS_7

#define DAC_LDAC_PORT   PORT_CHANNEL_E
#define DAC_LDAC_PIN    PORTS_BIT_POS_6

#define ADC_SPI_EN_PORT PORT_CHANNEL_B
#define ADC_SPI_EN_PIN  PORTS_BIT_POS_13

#define SPI1_SCK_PORT   PORT_CHANNEL_D
#define SPI1_SCK_PIN    PORTS_BIT_POS_2

#define SPI1_MOSI_PORT  PORT_CHANNEL_G
#define SPI1_MOSI_PIN   PORTS_BIT_POS_7

#define SPI1_MISO_PORT  PORT_CHANNEL_E
#define SPI1_MISO_PIN   PORTS_BIT_POS_5

#define CLEAR_SPI1_TX_INT   PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_SPI_1_TRANSMIT);
#define ENABLE_SPI1_TX_INT  PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_SPI_1_TRANSMIT);
#define DISABLE_SPI1_TX_INT PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_SPI_1_TRANSMIT);

#define CLEAR_SPI1_RX_INT   PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_SPI_1_RECEIVE);
#define ENABLE_SPI1_RX_INT  PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_SPI_1_RECEIVE);
#define DISABLE_SPI1_RX_INT PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_SPI_1_RECEIVE);

/*SPI Macros*/
#define SET_DAC_SPI_EN  PLIB_PORTS_PinSet(PORTS_ID_0, DAC_SPI_EN_PORT,DAC_SPI_EN_PIN)
#define CLEAR_DAC_SPI_EN PLIB_PORTS_PinClear(PORTS_ID_0, DAC_SPI_EN_PORT,DAC_SPI_EN_PIN)
#define SET_ADC_SPI_EN PLIB_PORTS_PinSet(PORTS_ID_0, ADC_SPI_EN_PORT,ADC_SPI_EN_PIN)
#define CLEAR_ADC_SPI_EN PLIB_PORTS_PinClear(PORTS_ID_0, ADC_SPI_EN_PORT,ADC_SPI_EN_PIN)
#define SET_DAC_LDAC    PLIB_PORTS_PinSet(PORTS_ID_0, DAC_LDAC_PORT,DAC_LDAC_PIN)
#define CLEAR_DAC_LDAC  PLIB_PORTS_PinClear(PORTS_ID_0, DAC_LDAC_PORT,DAC_LDAC_PIN)

//SPI Memory DMA Configuration
#define SPI_DACADC_TX_DMA_CHANNEL          DMA_CHANNEL_2
#define SPI_DACADC_RX_DMA_CHANNEL          DMA_CHANNEL_3
#define SPI_DACADC_TX_DESTINATION_ADDRESS  &SPI1BUF
#define SPI_DACADC_RX_SOURCE_ADDRESS       &SPI1BUF
#define SPI_DACADC_DMA_TX_START            PLIB_DMA_StartTransferSet(DMA_ID_0, SPI_DACADC_TX_DMA_CHANNEL)
#define SPI_DACADC_DMA_RX_START            PLIB_DMA_StartTransferSet(DMA_ID_0, SPI_DACADC_RX_DMA_CHANNEL)

#define CLEAR_SPI_DACADC_DMA_TX_INT        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_DMA_2)
#define ENABLE_SPI_DACADC_DMA_TX_INT       PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_DMA_2)
#define DISABLE_SPI_DACADC_DMA_TX_INT      PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_DMA_2)
#define CLEAR_SPI_DACADC_DMA_RX_INT        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_DMA_3)
#define ENABLE_SPI_DACADC_DMA_RX_INT       PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_DMA_3)
#define DISABLE_SPI_DACADC_DMA_RX_INT      PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_DMA_3)

/*Memory SPI Definitions*/
#define SPI_MEM             SPI_ID_2
#define SPI_MEM_BAUD_RATE   10000000 

#define SPI2_SCK_PORT   PORT_CHANNEL_G
#define SPI2_SCK_PIN    PORTS_BIT_POS_6

#define SPI2_MOSI_PORT  PORT_CHANNEL_G
#define SPI2_MOSI_PIN   PORTS_BIT_POS_8

#define SPI2_MISO_PORT  PORT_CHANNEL_B
#define SPI2_MISO_PIN   PORTS_BIT_POS_3

#define RAM_SPI_EN_PORT PORT_CHANNEL_B
#define RAM_SPI_EN_PIN  PORTS_BIT_POS_8

#define FLASH_SPI_EN_PORT   PORT_CHANNEL_B
#define FLASH_SPI_EN_PIN    PORTS_BIT_POS_2

#define SET_RAM_SPI_EN  PLIB_PORTS_PinSet(PORTS_ID_0, RAM_SPI_EN_PORT,RAM_SPI_EN_PIN)
#define CLEAR_RAM_SPI_EN PLIB_PORTS_PinClear(PORTS_ID_0, RAM_SPI_EN_PORT,RAM_SPI_EN_PIN)
#define SET_FLASH_SPI_EN PLIB_PORTS_PinSet(PORTS_ID_0, FLASH_SPI_EN_PORT,FLASH_SPI_EN_PIN)
#define CLEAR_FLASH_SPI_EN PLIB_PORTS_PinClear(PORTS_ID_0, FLASH_SPI_EN_PORT,FLASH_SPI_EN_PIN)

#define CLEAR_SPI2_TX_INT   PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_SPI_2_TRANSMIT);
#define ENABLE_SPI2_TX_INT  PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_SPI_2_TRANSMIT);
#define DISABLE_SPI2_TX_INT PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_SPI_2_TRANSMIT);
#define CLEAR_SPI2_RX_INT   PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_SPI_2_RECEIVE);
#define ENABLE_SPI2_RX_INT  PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_SPI_2_RECEIVE);
#define DISABLE_SPI2_RX_INT  PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_SPI_2_RECEIVE);

//SPI Memory DMA Configuration
#define SPI_MEM_TX_DMA_CHANNEL          DMA_CHANNEL_0
#define SPI_MEM_RX_DMA_CHANNEL          DMA_CHANNEL_1
#define SPI_MEM_TX_DESTINATION_ADDRESS  &SPI2BUF
#define SPI_MEM_RX_SOURCE_ADDRESS       &SPI2BUF
#define SPI_MEM_DMA_TX_START            PLIB_DMA_StartTransferSet(DMA_ID_0, SPI_MEM_TX_DMA_CHANNEL)
#define SPI_MEM_DMA_RX_START            PLIB_DMA_StartTransferSet(DMA_ID_0, SPI_MEM_RX_DMA_CHANNEL)


#define CLEAR_SPI_MEM_DMA_TX_INT        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_DMA_0)
#define ENABLE_SPI_MEM_DMA_TX_INT       PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_DMA_0)
#define DISABLE_SPI_MEM_DMA_TX_INT      PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_DMA_0)
#define CLEAR_SPI_MEM_DMA_RX_INT        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_DMA_1)
#define ENABLE_SPI_MEM_DMA_RX_INT       PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_DMA_1)
#define DISABLE_SPI_MEM_DMA_RX_INT      PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_DMA_1)

/*LEDs*/
#define SHIFT_REG_DATA_PORT  PORT_CHANNEL_B
#define SHIFT_REG_DATA_PIN   PORTS_BIT_POS_5

#define SHIFT_REG_CLK_PORT  PORT_CHANNEL_B
#define SHIFT_REG_CLK_PIN   PORTS_BIT_POS_4

#define LED_PWM_1_REMAP_PORT  OUTPUT_PIN_RPD1
#define LED_PWM_2_REMAP_PORT  OUTPUT_PIN_RPC14
#define LED_PWM_3_REMAP_PORT  OUTPUT_PIN_RPD3
#define LED_PWM_4_REMAP_PORT  OUTPUT_PIN_RPD4
#define LED_PWM_5_REMAP_PORT  OUTPUT_PIN_RPD5

#define RESET_SR_DATA   PLIB_PORTS_PinClear(PORTS_ID_0, SHIFT_REG_DATA_PORT, SHIFT_REG_DATA_PIN)
#define SET_SR_DATA     PLIB_PORTS_PinSet(PORTS_ID_0, SHIFT_REG_DATA_PORT, SHIFT_REG_DATA_PIN)
#define SET_SR_CLK      PLIB_PORTS_PinSet(PORTS_ID_0, SHIFT_REG_CLK_PORT, SHIFT_REG_CLK_PIN)
#define CLEAR_SR_CLK    PLIB_PORTS_PinClear(PORTS_ID_0, SHIFT_REG_CLK_PORT, SHIFT_REG_CLK_PIN)
#define TOGGLE_SR_CLK   PLIB_PORTS_PinToggle(PORTS_ID_0, SHIFT_REG_CLK_PORT, SHIFT_REG_CLK_PIN)

/*Switch*/
#define SWITCH_PORT                 PORT_CHANNEL_D
#define SWITCH_PIN                  PORTS_BIT_POS_11
#define SWITCH_PPS_PORT_PIN         INPUT_PIN_RPD11
#define CLEAR_SWITCH_INTERRUPT      PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_EXTERNAL_4)
#define ENABLE_SWITCH_INTERRUPT     PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_EXTERNAL_4)
#define DISABLE_SWITCH_INTERRUPT    PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_EXTERNAL_4)
#define SET_SWITCH_INTERRUPT_FALLING_EDGE   PLIB_INT_ExternalFallingEdgeSelect(INT_ID_0, INT_SOURCE_EXTERNAL_4)
#define SET_SWITCH_INTERRUPT_RISING_EDGE    PLIB_INT_ExternalRisingEdgeSelect(INT_ID_0, INT_SOURCE_EXTERNAL_4)
#define READ_SWITCH                 PLIB_PORTS_PinGet(PORTS_ID_0, SWITCH_PORT, SWITCH_PIN)

#define SWITCH_LED_PORT_1   PORT_CHANNEL_B
#define SWITCH_LED_PIN_1    PORTS_BIT_POS_6
#define SWITCH_LED_PORT_2   PORT_CHANNEL_B
#define SWITCH_LED_PIN_2    PORTS_BIT_POS_7

#define SET_SWITCH_LED_RED    PLIB_PORTS_PinSet(PORTS_ID_0, SWITCH_LED_PORT_1, SWITCH_LED_PIN_1); PLIB_PORTS_PinClear(PORTS_ID_0, SWITCH_LED_PORT_2, SWITCH_LED_PIN_2)
#define SET_SWITCH_LED_GREEN      PLIB_PORTS_PinClear(PORTS_ID_0, SWITCH_LED_PORT_1, SWITCH_LED_PIN_1); PLIB_PORTS_PinSet(PORTS_ID_0, SWITCH_LED_PORT_2, SWITCH_LED_PIN_2)
#define SET_SWITCH_LED_OFF       PLIB_PORTS_PinSet(PORTS_ID_0, SWITCH_LED_PORT_1, SWITCH_LED_PIN_1); PLIB_PORTS_PinSet(PORTS_ID_0, SWITCH_LED_PORT_2, SWITCH_LED_PIN_2)

#define ROTARY_ENC_A_PORT       PORT_CHANNEL_E
#define ROTARY_ENC_A_PIN        PORTS_BIT_POS_3
#define ROTARY_ENC_B_PORT       PORT_CHANNEL_E
#define ROTARY_ENC_B_PIN        PORTS_BIT_POS_4
#define ROTARY_ENC_INT_SOURCE   INT_SOURCE_CHANGE_NOTICE_E
#define ENABLE_PORT_E_CHANGE_INTERRUPT    PLIB_INT_SourceEnable(INT_ID_0, ROTARY_ENC_INT_SOURCE);
#define DISABLE_PORT_E_CHANGE_INTERRUPT   PLIB_INT_SourceDisable(INT_ID_0, ROTARY_ENC_INT_SOURCE);
#define CLEAR_PORT_E_CHANGE_INTERRUPT     PLIB_INT_SourceFlagClear(INT_ID_0, ROTARY_ENC_INT_SOURCE);
#define READ_ENCODER_A          PLIB_PORTS_PinGet(PORTS_ID_0, ROTARY_ENC_A_PORT, ROTARY_ENC_A_PIN);
#define READ_ENCODER_B          PLIB_PORTS_PinGet(PORTS_ID_0, ROTARY_ENC_B_PORT, ROTARY_ENC_B_PIN);

#define ROTARY_ENC_SWITCH_PORT  PORT_CHANNEL_B
#define ROTARY_ENC_SWITCH_PIN   PORTS_BIT_POS_9
#define ROTARY_ENC_SWITCH_INT_SOURCE    INT_SOURCE_EXTERNAL_3
#define ENABLE_ROTARY_ENC_SWITCH_INT    PLIB_INT_SourceEnable(INT_ID_0, ROTARY_ENC_SWITCH_INT_SOURCE)
#define DISABLE_ROTARY_ENC_SWITCH_INT   PLIB_INT_SourceDisable(INT_ID_0, ROTARY_ENC_SWITCH_INT_SOURCE)
#define CLEAR_ROTARY_ENC_SWITCH_INT     PLIB_INT_SourceFlagClear(INT_ID_0, ROTARY_ENC_SWITCH_INT_SOURCE)
#define ROTARY_ENC_SWITCH_PPS           INPUT_PIN_RPB9
#define READ_ENC_SWITCH PLIB_PORTS_PinGet(PORTS_ID_0, ROTARY_ENC_SWITCH_PORT, ROTARY_ENC_SWITCH_PIN)


/*GPIO Functionality*/
#define RECORD_IN_PORT  PORT_CHANNEL_E
#define RECORD_IN_PIN   PORTS_BIT_POS_2
#define LOOP_SYNC_PORT  PORT_CHANNEL_F
#define LOOP_SYNC_PIN   PORTS_BIT_POS_0
#define SYNC_IN_PORT  PORT_CHANNEL_E//Becomes sync
#define SYNC_IN_PIN   PORTS_BIT_POS_0
#define PLAY_IN_PORT    PORT_CHANNEL_F//Becomes play input
#define PLAY_IN_PIN     PORTS_BIT_POS_1
#define GATE_OUT_PORT   PORT_CHANNEL_E
#define GATE_OUT_PIN    PORTS_BIT_POS_1
#define HOLD_PORT       PORT_CHANNEL_D
#define HOLD_PIN        PORTS_BIT_POS_7
#define ENABLE_PORT_F_CHANGE_INTERRUPT    PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_CHANGE_NOTICE_F);
#define DISABLE_PORT_F_CHANGE_INTERRUPT   PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_CHANGE_NOTICE_F);
#define CLEAR_PORT_F_CHANGE_INTERRUPT     PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_CHANGE_NOTICE_F);
#define ENABLE_PORT_D_CHANGE_INTERRUPT    PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_CHANGE_NOTICE_D);
#define DISABLE_PORT_D_CHANGE_INTERRUPT   PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_CHANGE_NOTICE_D);
#define CLEAR_PORT_D_CHANGE_INTERRUPT     PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_CHANGE_NOTICE_D);
#define SET_GATE_OUT_PORT PLIB_PORTS_PinClear(PORTS_ID_0, GATE_OUT_PORT, GATE_OUT_PIN);
#define CLEAR_GATE_OUT_PORT PLIB_PORTS_PinSet(PORTS_ID_0, GATE_OUT_PORT, GATE_OUT_PIN);
#define SET_LOOP_SYNC_OUT   PLIB_PORTS_PinClear(PORTS_ID_0, LOOP_SYNC_PORT, LOOP_SYNC_PIN);
#define CLEAR_LOOP_SYNC_OUT PLIB_PORTS_PinSet(PORTS_ID_0, LOOP_SYNC_PORT, LOOP_SYNC_PIN);

#define JACK_DETECT_PORT    PORT_CHANNEL_D
#define JACK_DETECT_PIN     PORTS_BIT_POS_8
#define READ_JACK_DETECT_PIN    PLIB_PORTS_PinGet(PORTS_ID_0, JACK_DETECT_PORT, JACK_DETECT_PIN)
#define MODULATION_JACK_PLUGGED_IN  !READ_JACK_DETECT_PIN


#define LED_TIMER_ID    TMR_ID_2
/*Blue LED PWMs*/
#define LED_PWM1_OC_ID  OC_ID_1
#define LED_PWM2_OC_ID  OC_ID_2
#define LED_PWM3_OC_ID  OC_ID_3
#define LED_PWM4_OC_ID  OC_ID_4
/*Red LED PWM*/
#define LED_PWM5_OC_ID  OC_ID_5

#define MAX_BRIGHTNESS          512//Sets the timer frequency to
#define HALF_BRIGHTNESS         MAX_BRIGHTNESS>>1

#define RESET_LED_TIMER PLIB_TMR_Counter16BitClear(LED_TIMER_ID);

/*Timer for timing the clock pulses - Its speed is variable along with
 * the speed of the
 */
#define CLOCK_TIMER_ID  TMR_ID_3
#define CLOCK_TIMER_PRESCALE    TMR_PRESCALE_VALUE_1
#define CLOCK_TIMER_PERIOD  SAMPLE_TIMER_PERIOD>>3 //Start out with /8
#define CLOCK_TIMER_INT_SOURCE INT_SOURCE_TIMER_3
#define ENABLE_CLOCK_TIMER_INT  PLIB_INT_SourceEnable(INT_ID_0, CLOCK_TIMER_INT_SOURCE)
#define DISABLE_CLOCK_TIMER_INT PLIB_INT_SourceDisable(INT_ID_0, CLOCK_TIMER_INT_SOURCE)
#define CLEAR_CLOCK_TIMER_INT   PLIB_INT_SourceFlagClear(INT_ID_0, CLOCK_TIMER_INT_SOURCE)
#define RESET_CLOCK_TIMER  PLIB_TMR_Counter16BitClear(CLOCK_TIMER_ID)
#define START_CLOCK_TIMER   PLIB_TMR_Start(CLOCK_TIMER_ID)
#define STOP_CLOCK_TIMER    PLIB_TMR_Stop(CLOCK_TIMER_ID)
#define SET_CLOCK_TIMER_PRESCALE(newValue)    PLIB_TMR_PrescaleSelect(CLOCK_TIMER_ID, newValue)
#define SET_CLOCK_TIMER_PERIOD(newValue) PLIB_TMR_Period16BitSet(CLOCK_TIMER_ID, newValue);
#define MAX_CLOCK_TIMER_SPEED   0xFFFF

#define OSAL_USE_RTOS
				#define OSAL_MAX_PRIORITIES     10
				#define OSAL_DAEMON_STACK_SIZE 1024

#endif // _SYS_CONFIG_H
/*******************************************************************************
 End of File
*/


