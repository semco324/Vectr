#include "peripheral/peripheral.h"
#include "system_config.h"

void BSP_Initialize(void )
{
    ANSELD = 0;
    ANSELB = 0;
    ANSELE = 0;
    ANSELF = 0;
    
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, SHIFT_REG_DATA_PORT, SHIFT_REG_DATA_PIN);
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, SHIFT_REG_CLK_PORT, SHIFT_REG_CLK_PIN);

    clear_led_shift_registers();

    /*Configure the MGC3130 Reset Pin - Active Low*/
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, MGC3130_RESET_PORT, MGC3130_RESET_PIN);
    MGC3130_CLEAR_RESET;

    /*Configure the MGC3130 TS Pin - Open Drain*/
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, MGC3130_TS_PORT, MGC3130_TS_PIN);
    PLIB_PORTS_PinOpenDrainEnable(PORTS_ID_0, MGC3130_TS_PORT, MGC3130_TS_PIN);
    MGC3130_TS_PIN_HIGH;

    /*Configure SPI Enable Pin outputs*/
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, DAC_SPI_EN_PORT, DAC_SPI_EN_PIN);
    SET_DAC_SPI_EN;

    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, DAC_LDAC_PORT, DAC_LDAC_PIN);
    CLEAR_DAC_LDAC;

    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, ADC_SPI_EN_PORT, ADC_SPI_EN_PIN);
    SET_ADC_SPI_EN;

    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, SPI1_MISO_PORT, SPI1_MISO_PIN);
    PLIB_PORTS_PinOpenDrainDisable(PORTS_ID_0, SPI1_MISO_PORT, SPI1_MISO_PIN);

    /*Configure MOSI and MISO for SPI1*/
    PLIB_PORTS_RemapOutput(PORTS_ID_0, OTPUT_FUNC_SDO1, OUTPUT_PIN_RPG7);
    PLIB_PORTS_RemapInput(PORTS_ID_0, INPUT_FUNC_SDI1, INPUT_PIN_RPE5);

    /*Configure MOSI and MISO for SPI2*/
    PLIB_PORTS_RemapOutput(PORTS_ID_0, OTPUT_FUNC_SDO2, OUTPUT_PIN_RPG8);
    PLIB_PORTS_RemapInput(PORTS_ID_0, INPUT_FUNC_SDI2, INPUT_PIN_RPB3);

    /*Configure SPI2 Enable Pin Outputs*/
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, RAM_SPI_EN_PORT, RAM_SPI_EN_PIN);
    SET_RAM_SPI_EN;
    
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, FLASH_SPI_EN_PORT, FLASH_SPI_EN_PIN);
    SET_FLASH_SPI_EN;

    /*Configure Switch External Interrupt*/
    PLIB_PORTS_RemapInput(PORTS_ID_0, INPUT_FUNC_INT4, SWITCH_PPS_PORT_PIN);

    /*Configure Rotary Encoder Inputs*/
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, ROTARY_ENC_A_PORT, ROTARY_ENC_A_PIN);
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, ROTARY_ENC_B_PORT, ROTARY_ENC_B_PIN);

    /*Configure rotary encoder switch external interrupt*/
    PLIB_PORTS_RemapInput(PORTS_ID_0, INPUT_FUNC_INT3, ROTARY_ENC_SWITCH_PPS);

    /*Configure other I/O*/
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, RECORD_IN_PORT, RECORD_IN_PIN);
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PLAY_IN_PORT, PLAY_IN_PIN);
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, SYNC_IN_PORT, SYNC_IN_PIN);
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, HOLD_PORT, HOLD_PIN);
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, JACK_DETECT_PORT, JACK_DETECT_PIN);
    PLIB_PORTS_ChangeNoticeInIdlePerPortEnable(PORTS_ID_0, JACK_DETECT_PORT);
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, LOOP_SYNC_PORT, LOOP_SYNC_PIN);
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, GATE_OUT_PORT, GATE_OUT_PIN);
    PLIB_PORTS_OpenDrainEnable(PORTS_ID_0, GATE_OUT_PORT, GATE_OUT_PIN);
  //  PLIB_PORTS_OpenDrainEnable(PORTS_ID_0, LOOP_SYNC_PORT, LOOP_SYNC_PIN);
    ODCFbits.ODCF0 = 1;
    ODCEbits.ODCE1 = 1;
    PLIB_PORTS_PinClear(PORTS_ID_0, GATE_OUT_PORT, GATE_OUT_PIN);
    PLIB_PORTS_PinClear(PORTS_ID_0, LOOP_SYNC_PORT, LOOP_SYNC_PIN);

    /*Configure the pin change interrupts*/
    CNCONEbits.ON = 1;
    PLIB_PORTS_PinChangeNoticePerPortEnable(PORTS_ID_0, ROTARY_ENC_A_PORT, ROTARY_ENC_A_PIN);
    PLIB_PORTS_PinChangeNoticePerPortEnable(PORTS_ID_0, ROTARY_ENC_B_PORT, ROTARY_ENC_B_PIN);
    PLIB_PORTS_PinChangeNoticePerPortEnable(PORTS_ID_0, RECORD_IN_PORT, RECORD_IN_PIN);
    PLIB_PORTS_PinChangeNoticePerPortEnable(PORTS_ID_0, PLAY_IN_PORT, PLAY_IN_PIN);

    CNCONFbits.ON = 1;
    PLIB_PORTS_PinChangeNoticePerPortEnable(PORTS_ID_0, SYNC_IN_PORT, SYNC_IN_PIN);

    CNCONDbits.ON = 1;
    PLIB_PORTS_PinChangeNoticePerPortEnable(PORTS_ID_0, HOLD_PORT, HOLD_PIN);
    PLIB_PORTS_PinChangeNoticePerPortEnable(PORTS_ID_0, JACK_DETECT_PORT, JACK_DETECT_PIN);

    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, SWITCH_LED_PORT_1, SWITCH_LED_PIN_1);
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, SWITCH_LED_PORT_2, SWITCH_LED_PIN_2);
    SET_SWITCH_LED_GREEN;

    /*Configure LED stuff*/
    
    PLIB_PORTS_RemapOutput(PORTS_ID_0, OTPUT_FUNC_OC1, LED_PWM_1_REMAP_PORT);
    PLIB_PORTS_RemapOutput(PORTS_ID_0, OTPUT_FUNC_OC2, LED_PWM_2_REMAP_PORT);
    PLIB_PORTS_RemapOutput(PORTS_ID_0, OTPUT_FUNC_OC3, LED_PWM_3_REMAP_PORT);
    PLIB_PORTS_RemapOutput(PORTS_ID_0, OTPUT_FUNC_OC4, LED_PWM_4_REMAP_PORT);
    PLIB_PORTS_RemapOutput(PORTS_ID_0, OTPUT_FUNC_OC5, LED_PWM_5_REMAP_PORT);


    /*Turn on the Timer 4/5 for master timing control in 32 bit mode*/
    PLIB_TMR_Mode32BitEnable(SAMPLE_TIMER);
    PLIB_TMR_PrescaleSelect(SAMPLE_TIMER, SAMPLE_TIMER_PRESCALE);
    PLIB_TMR_ClockSourceSelect(SAMPLE_TIMER, TMR_CLOCK_SOURCE_PERIPHERAL_CLOCK);
    PLIB_TMR_Period32BitSet(SAMPLE_TIMER, SAMPLE_TIMER_PERIOD);
    PLIB_TMR_Start(SAMPLE_TIMER);

    /*Turn on the timer for pulse timing.*/
    PLIB_TMR_PrescaleSelect(PULSE_TIMER_ID, PULSE_TIMER_PRESCALE);
    PLIB_TMR_ClockSourceSelect(PULSE_TIMER_ID, TMR_CLOCK_SOURCE_PERIPHERAL_CLOCK);
    PLIB_TMR_Period16BitSet(PULSE_TIMER_ID, PULSE_TIMER_PERIOD);
    PLIB_TMR_Start(PULSE_TIMER_ID);

    /*Turn on the Timer 2 for the LED PWM*/
    PLIB_TMR_PrescaleSelect(LED_TIMER_ID, TMR_PRESCALE_VALUE_1);
    PLIB_TMR_ClockSourceSelect(LED_TIMER_ID, TMR_CLOCK_SOURCE_PERIPHERAL_CLOCK);
    PLIB_TMR_Period16BitSet(LED_TIMER_ID, MAX_BRIGHTNESS);
    PLIB_TMR_Start(LED_TIMER_ID);

    /*Configure each of the LED PWMs*/
    PLIB_OC_TimerSelect(LED_PWM1_OC_ID, OC_TIMER_16BIT_TMR2);
    PLIB_OC_ModeSelect(LED_PWM1_OC_ID, OC_COMPARE_PWM_EDGE_ALIGNED_MODE );
    PLIB_OC_PulseWidth16BitSet( LED_PWM1_OC_ID, 0x0000);
    PLIB_OC_Enable(LED_PWM1_OC_ID);

    PLIB_OC_TimerSelect(LED_PWM2_OC_ID, OC_TIMER_16BIT_TMR2);
    PLIB_OC_ModeSelect(LED_PWM2_OC_ID, OC_COMPARE_PWM_EDGE_ALIGNED_MODE );
    PLIB_OC_PulseWidth16BitSet( LED_PWM2_OC_ID, 0x0000);
    PLIB_OC_Enable(LED_PWM2_OC_ID);

    PLIB_OC_TimerSelect(LED_PWM3_OC_ID, OC_TIMER_16BIT_TMR2);
    PLIB_OC_ModeSelect(LED_PWM3_OC_ID, OC_COMPARE_PWM_EDGE_ALIGNED_MODE );
    PLIB_OC_PulseWidth16BitSet( LED_PWM3_OC_ID, 0x0000);
    PLIB_OC_Enable(LED_PWM3_OC_ID);

    PLIB_OC_TimerSelect(LED_PWM4_OC_ID, OC_TIMER_16BIT_TMR2);
    PLIB_OC_ModeSelect(LED_PWM4_OC_ID, OC_COMPARE_PWM_EDGE_ALIGNED_MODE );
    PLIB_OC_PulseWidth16BitSet( LED_PWM4_OC_ID, 0x0000);
    PLIB_OC_Enable(LED_PWM4_OC_ID);

    PLIB_OC_TimerSelect(LED_PWM5_OC_ID, OC_TIMER_16BIT_TMR2);
    PLIB_OC_ModeSelect(LED_PWM5_OC_ID, OC_COMPARE_PWM_EDGE_ALIGNED_MODE );
    PLIB_OC_PulseWidth16BitSet( LED_PWM5_OC_ID, 0x0000);
    PLIB_OC_Enable(LED_PWM5_OC_ID);


}
