#include "system_config.h"
#include "menu.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "leds.h"
#include "master_control.h"
#include "mem.h"

static  uint16_t u16encState;
static  int16_t  i16value;
static uint8_t u8encALastState,
                u8encBLastState;
static int8_t i8MainMenuState;
static int16_t i16EncoderState = 0;
static int8_t i8SubMenuState = SUBMENU_NOT_SELECTED;
static int8_t i8MenuChangeFlag = FALSE;
static uint8_t u8MenuKeyPressFlag = FALSE;
static uint8_t u8ParameterEditModeFlag = FALSE;
static uint8_t u8MenuExitFlag = FALSE;
static uint8_t u8LoadModeFlag = FALSE;
static uint8_t u8StoreModeFlag = FALSE;
static uint8_t u8LoadStoreParameter = 0;
static uint8_t u8EffectModeFlag = FALSE;
static uint8_t u8ClockModeFlag = FALSE;
static int16_t i16EncoderLiveZeroPosition = 0;
static uint8_t u8MenuEntryParameter = 0;//Stored so that it can be recalled if no change is desired
static uint8_t u8LoadStoreActivePositions[MAX_NUM_OF_LOAD_STORE_LOCATIONS] = 
{FALSE,
FALSE,
FALSE,
FALSE,
FALSE};


const uint8_t u8LEDMainMenuMapping[NUM_OF_MAIN_MENUS] =
{   LED20,
    LED19,
    LED18,
    LED17,
    LED16,
    LED6,
    LED7,
    LED8,
    LED9,
    LED10};

const uint8_t u8XYZSubMenuMapping[NUM_OF_XYZ_SUBMENUS] =
{
  LED6,
  LED7,
  LED8,
  LED9,
  LED10
};

const uint8_t u8StandardSubMenuMapping[MAX_NUM_OF_LOAD_STORE_LOCATIONS] =
{
  LED1,
  LED2,
  LED3,
  LED4,
  LED5
};

const uint8_t u8PlaySubMenuMapping[5] =
{
    LED20,
    LED19,
    LED18,
    LED17,
    LED16
};

const uint8_t u8BottomParameterMapping[NUM_OF_BOTTOM_PARAMETERS] =
{
    LED15,
    LED14,
    LED13,
    LED12,
    LED11
};

void changeMainMenuState(void);
void changeXYZSubMenuState(void);
void editXYZParameter(uint8_t u8Parameter);
void initializeXYZParameterMode(void);
void initializeRecParameterMode(uint8_t u8RecPlayOdub);
void editRecParameter(uint8_t u8RecPlayOdub);
void editLoadStoreParameter(void);
void changeRecSubMenuState(void);
void initializeClockMode(void);
void editClockParameter(void);
void initializeEffectMode(void);
void editEffectParameter(void);
void determineLoadStorePositions(void);
uint8_t initializeLoadStoreMode(void);
void resetEncoderLiveInteraction(void);
void indicateLEDContinuousXYZParameter(uint8_t u8Parameter, uint16_t u16Value);
void resetMenuParameter(void);
void resetClockParameter(void);
void resetRecParameter(uint8_t u8RecPlayOdub);
void resetEffectParameter(void);
void resetXYZParameter(uint8_t u8Mode);
void changeClockSubMenuState(void);

uint16_t getEncState(void){
    return u16encState;
}

void setEncState(uint16_t u16newState){
    u16encState = u16newState;
}

void menuHandlerInit(void){
    //In order to ensure the LEDs get cleared
    i8MainMenuState = X_MENU;
    setLEDAlternateFuncFlag(TRUE);
    turnOffAllLEDs();
    i8MenuChangeFlag = 0;
    vTaskDelay(500);//Make sure the LED queue gets cleared
    setLEDState(u8LEDMainMenuMapping[i8MainMenuState], BLINK);
    setRedLEDs(MAX_BRIGHTNESS);
}

void MenuStateMachine(void){
    uint8_t u8ExitWithoutSavingFlag = TRUE;
    uint8_t u8MemCommand;
    static uint32_t u32defaultSettingsInitialValue;
    static uint8_t u8checkForDefaultSettings = FALSE;
    uint32_t u32TickCount;

    //Check to see if the encoder moved
    if(i8MenuChangeFlag != 0 && i8SubMenuState == SUBMENU_NOT_SELECTED){
        changeMainMenuState();
        resetBlink();
    }

    //Check to see if the encoder switch is held down for 3 seconds. This is
    //the mechanism for defaulting the menu.
    if(u8MenuKeyPressFlag == TRUE){
        //Start the timer.
        u32TickCount = xTaskGetTickCount();
        u32defaultSettingsInitialValue = u32TickCount;
        u8checkForDefaultSettings = TRUE;
    }else{
        //If the switch continues to be held down for 3 seconds, default the settings and exit the menu.
        if(u8checkForDefaultSettings == TRUE){
            if(!READ_ENC_SWITCH){//Switch is held down.
                //Increment the timer
                u32TickCount = xTaskGetTickCount();
                if((u32TickCount - u32defaultSettingsInitialValue) > DEFAULT_SETTINGS_TIME_LENGTH){
                    defaultSettings();
                    u8ExitWithoutSavingFlag = FALSE;
                    u8MenuExitFlag = TRUE;
                    u8checkForDefaultSettings = FALSE;
                }
            }else{
                u8checkForDefaultSettings = FALSE;
            }
        }
    }

    switch(i8MainMenuState){
        case X_MENU:
        case Y_MENU:
        case Z_MENU:
            //Handle the encoder switch press
            if(u8MenuKeyPressFlag == TRUE){
                u8MenuKeyPressFlag = FALSE;
                //If no submenu has been selected, enter submenu mode
                if(i8SubMenuState == SUBMENU_NOT_SELECTED){
                    i8SubMenuState = RANGE;
                    setLEDState(u8LEDMainMenuMapping[i8MainMenuState], ON);
                    setLEDState(u8XYZSubMenuMapping[i8SubMenuState-1], BLINK);
                }
                else if(u8ParameterEditModeFlag == FALSE){
                    //Enter the submenu and modify the parameter
                    u8ParameterEditModeFlag = TRUE;
                    setLEDState(u8XYZSubMenuMapping[i8SubMenuState-1], ON);
                    initializeXYZParameterMode();
                }
                else{
                    //Lock in the selection and exit the menu.
                    u8ExitWithoutSavingFlag = FALSE;
                    u8MenuExitFlag = TRUE;
                }
            }

            //If it made it this far, then it's changing submenus
            if(i8MenuChangeFlag != 0){
                //If we're not in parameter edit mode
                if(u8ParameterEditModeFlag == FALSE){
                    changeXYZSubMenuState();
                    resetBlink();
                }
                else{
                    editXYZParameter(i8SubMenuState);
                }
            }  
            break;
        case RECORD_MENU:
        case OVERDUB_MENU:
        case PLAYBACK_MENU:
            //Handle the encoder switch press
            if(u8MenuKeyPressFlag == TRUE){
                u8MenuKeyPressFlag = FALSE;
                //If no submenu has been selected, enter submenu mode
                if(i8SubMenuState == SUBMENU_NOT_SELECTED){
                    i8SubMenuState = SOURCE;
                    setLEDState(u8LEDMainMenuMapping[i8MainMenuState], ON);
                    setLEDState(u8PlaySubMenuMapping[i8SubMenuState-1], BLINK);
                }
                else if(u8ParameterEditModeFlag == FALSE){
                    //Enter the submenu and modify the parameter
                    u8ParameterEditModeFlag = TRUE;
                    setLEDState(u8PlaySubMenuMapping[i8SubMenuState-1], ON);
                    initializeRecParameterMode(i8MainMenuState-RECORD_MENU);
                }
                else{
                    //Locking in the change and exiting the menu.
                    u8ExitWithoutSavingFlag = FALSE;
                    u8MenuExitFlag = TRUE;
                }
            }

            //If it made it this far, then it's changing submenus
            if(i8MenuChangeFlag != 0){
                //If we're not in parameter edit mode
                if(u8ParameterEditModeFlag == FALSE){
                    changeRecSubMenuState();
                    resetBlink();
                }
                else{
                    editRecParameter(i8MainMenuState-RECORD_MENU);
                }
            }
            break;
        case LOAD_MENU:
        case STORE_MENU:
            //Store a sequence
            /*The first key press enters the mode. The next one issues a store
             * command*/
            if(u8MenuKeyPressFlag == TRUE){
                u8MenuKeyPressFlag = FALSE;
                if(i8MainMenuState == LOAD_MENU){
                    if(u8LoadModeFlag == FALSE){
                        //u8LoadModeFlag = TRUE;
                        /*Set the LEDs on for locations which have a stored sequence. Flash 
                         the currently selected LED.*/
                        u8LoadModeFlag = initializeLoadStoreMode();
                        if(u8LoadModeFlag == 1){
                            i8SubMenuState = 1;
                        }
                    }
                    else{
                        //Load the sequence
                        flashLoadSequence(u8LoadStoreParameter);
                        u8LoadModeFlag = FALSE;
                        u8MenuExitFlag = TRUE;
                    }
                }else{
                    if(u8StoreModeFlag == FALSE){
                        //u8StoreModeFlag = TRUE;
                        u8StoreModeFlag = initializeLoadStoreMode();
                        if(u8StoreModeFlag == 1){
                            i8SubMenuState = 1;
                        }
                    }
                    else{
                        //Store the sequence
                        flashStoreSequence(u8LoadStoreParameter, getLengthOfRecording());
                        u8StoreModeFlag = FALSE;
                        u8MenuExitFlag = TRUE;
                    }
                }
            }

            //Handle the encoder
            if(u8StoreModeFlag == TRUE || u8LoadModeFlag == TRUE){
                if(i8MenuChangeFlag != 0){
                    editLoadStoreParameter();
                }
            }
            else{
                i8MenuChangeFlag = 0;
            }

            break;
        case EFFECT_MENU:
            if(u8MenuKeyPressFlag == TRUE){
                u8MenuKeyPressFlag = FALSE;

                if(u8EffectModeFlag == FALSE){
                    i8SubMenuState = 1;
                    u8EffectModeFlag = TRUE;
                    setLEDState(u8LEDMainMenuMapping[i8MainMenuState], ON);
                    initializeEffectMode();
                }
                else{
                    //Set the effect mode
                    u8EffectModeFlag = FALSE;
                    //Locking in the change and exiting the menu.
                    u8ExitWithoutSavingFlag = FALSE;
                    u8MenuExitFlag = TRUE;
                }

            }

            //Handle the encoder
            if(u8EffectModeFlag == TRUE){
                if(i8MenuChangeFlag != 0){
                    editEffectParameter();
                }
            }
            else{
                i8MenuChangeFlag = 0;
            }
            break;
        case CLOCK_MENU:
            if(u8MenuKeyPressFlag == TRUE){
                u8MenuKeyPressFlag = FALSE;

                if(i8SubMenuState == SUBMENU_NOT_SELECTED){
                    u8ClockModeFlag = TRUE;
                    i8SubMenuState = SYNC;
                    setLEDState(u8LEDMainMenuMapping[i8MainMenuState], ON);
                    setLEDState(u8LEDMainMenuMapping[i8SubMenuState-1], BLINK);
                }
                else if(u8ParameterEditModeFlag == FALSE){
                    //Enter the submenu and modify the parameter
                    u8ParameterEditModeFlag = TRUE;
                    setLEDState(u8LEDMainMenuMapping[i8SubMenuState-1], ON);
                    initializeClockMode();
                }
                else{
                    //Set the effect mode
                    u8ClockModeFlag = FALSE;
                    //Locking in the change and exiting the menu.
                    u8ExitWithoutSavingFlag = FALSE;
                    u8MenuExitFlag = TRUE;
                }

            }

            //If it made it this far, then it's changing submenus
            if(i8MenuChangeFlag != 0){
                //If we're not in parameter edit mode
                if(u8ParameterEditModeFlag == FALSE){
                    changeClockSubMenuState();
                    resetBlink();
                }
                else{
                    editClockParameter();
                }
            }
            break;
        default:
            break;   
    }


    /*When we exit the menu, we have to reset the LIVE PLAY INTERACTION stuff or
     we could end up in a weird state.*/

    if(u8MenuExitFlag == TRUE){
        /*If the main switch is pressed, then we exit without saving the change and
         need to reset the parameter.*/
        if(u8ExitWithoutSavingFlag == TRUE){
            resetMenuParameter();
        }
        else{
            //Write the file table with the stored settings.
            copyCurrentSettingsToFileTable(0);
            u8MemCommand = WRITE_FLASH_FILE_TABLE;
            xQueueSend(xMemInstructionQueue, &u8MemCommand, 0);
        }
        resetEncoderLiveInteraction();//Encoder turning needs to be reset to not get spurious changes.
        i8SubMenuState = SUBMENU_NOT_SELECTED;
        u8MenuExitFlag = FALSE;
        setLEDAlternateFuncFlag(FALSE);//LEDs back to indicating position
        turnOffAllLEDs();//Reset the LEDs state
        setRedLEDs(HALF_BRIGHTNESS);
        setMenuModeFlag(FALSE);//Master control knows menu is over
        u8ParameterEditModeFlag = FALSE;
        u8LoadModeFlag = FALSE;
        u8StoreModeFlag = FALSE;
    }
}

#define DEBOUNCE_TIMER_RESET    35

/*The encoder can be used when not in menu mode to return to live play from
 * another mode by turning two clicks to the right. A hold command can be toggled
 * by turning the encoder two clicks to the left. The first turn to the left will
 * activate a hold and the next turn will deactivate it.
 */
void encoderLiveInteraction(void){
    io_event_message event_message;
    static uint8_t u8debounceTimer = 0;

    if(u8debounceTimer > 0){
        u8debounceTimer--;
    }
    
    if(u8debounceTimer == 0){
        if(i16EncoderState >= i16EncoderLiveZeroPosition+LIVE_PLAY_ACTIVATION_CLICKS){
            setLivePlayActivationFlag();
            i16EncoderLiveZeroPosition = i16EncoderState;
           // setClockEnableFlag(FALSE);
            u8debounceTimer = DEBOUNCE_TIMER_RESET;
        }
        else if(i16EncoderState <= i16EncoderLiveZeroPosition-HOLD_ACTIVATION_CLICKS){

            if(getHoldState() == OFF){
                event_message.u16messageType = HOLD_IN_EVENT;
                event_message.u16message = TRIGGER_WENT_HIGH;
                xQueueSend(xIOEventQueue, &event_message, 0);
            }
            else{
                event_message.u16messageType = HOLD_IN_EVENT;
                event_message.u16message = TRIGGER_WENT_LOW;
                xQueueSend(xIOEventQueue, &event_message, 0);
            }
            i16EncoderLiveZeroPosition = i16EncoderState;
            u8debounceTimer = DEBOUNCE_TIMER_RESET;
        }
    }else{
       i16EncoderLiveZeroPosition = i16EncoderState;
    }
    
}

void indicateLEDContinuousXYZParameter(uint8_t u8Parameter, uint16_t u16Value){
    //Set the five top LEDs according to the parameter.

    uint16_t u16MaxValue;
    uint16_t u16SecondLEDValue;
    uint16_t u16ThirdLEDValue;
    uint16_t u16FourthLEDValue;
    uint16_t u16LEDIncrement;
    uint16_t u16LEDBrightness;
    uint16_t u16LEDScaling;
    uint8_t u8LEDIndex = 0;

    switch(u8Parameter){
        case LINEARITY:
            u16MaxValue = LINEARITY_MAX;
            u16LEDIncrement = LINEARITY_LED_INCREMENT;
            u16SecondLEDValue =  u16LEDIncrement;
            u16ThirdLEDValue = u16SecondLEDValue + u16LEDIncrement;
            u16FourthLEDValue = u16ThirdLEDValue + u16LEDIncrement;
            u16LEDScaling = LINEARITY_LED_SCALING;
            break;
        case SLEW_RATE:
            u16MaxValue = MAX_SLEW_RATE;
            u16LEDIncrement = SLEW_RATE_LED_INCREMENT;
            u16SecondLEDValue =  u16LEDIncrement;
            u16ThirdLEDValue = u16SecondLEDValue + u16LEDIncrement;
            u16FourthLEDValue = u16ThirdLEDValue + u16LEDIncrement;
            u16LEDScaling = SLEW_RATE_LED_SCALING;
            break;
        default:
            break;
    }

    //Set the brightness for each LED

    /* All the way at the LED makes the LED max brightness,
     * halfway between makes both LEDs half brightness.
     *
     * LED 0 = 0
     * LED 1 = 1/4
     * LED 2 = 1/2
     * LED 3 = 3/4
     * LED 4 = MAX
     */

    //LED1 and 2
    if(u16Value < u16SecondLEDValue){
        u16LEDBrightness = u16SecondLEDValue - u16Value;
        if(u8Parameter == LINEARITY){
            u16LEDBrightness *= u16LEDScaling;
        }
        else{
            u16LEDBrightness >>= u16LEDScaling;
        }
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex], u16LEDBrightness);
        u8LEDIndex++;
        u16LEDBrightness = MAX_BRIGHTNESS - u16LEDBrightness;
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex++], u16LEDBrightness);
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex++], 0);
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex++], 0);
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex], 0);
    }
    else if(u16Value < u16ThirdLEDValue){
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex], 0);
        u8LEDIndex++;
        u16LEDBrightness = u16ThirdLEDValue - u16Value;
        if(u8Parameter == LINEARITY){
            u16LEDBrightness *= u16LEDScaling;
        }
        else{
            u16LEDBrightness >>= u16LEDScaling;
        }
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex], u16LEDBrightness);
        u8LEDIndex++;
        u16LEDBrightness = MAX_BRIGHTNESS - u16LEDBrightness;
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex++], u16LEDBrightness);
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex++], 0);
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex], 0);
    }
    else if(u16Value < u16FourthLEDValue){
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex++], 0);
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex++], 0);
        u16LEDBrightness = u16FourthLEDValue - u16Value;
        if(u8Parameter == LINEARITY){
            u16LEDBrightness *= u16LEDScaling;
        }
        else{
            u16LEDBrightness >>= u16LEDScaling;
        }
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex], u16LEDBrightness);
        u8LEDIndex++;
        u16LEDBrightness = MAX_BRIGHTNESS - u16LEDBrightness;
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex++], u16LEDBrightness);
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex], 0);
    }
    else{
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex++], 0);
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex++], 0);
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex++], 0);
        u16LEDBrightness = u16MaxValue - u16Value;
        if(u8Parameter == LINEARITY){
            u16LEDBrightness *= u16LEDScaling;
        }
        else{
            u16LEDBrightness >>= u16LEDScaling;
        }
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex], u16LEDBrightness);
        u8LEDIndex++;
        u16LEDBrightness = MAX_BRIGHTNESS - u16LEDBrightness;
        setBlueLEDBrightness(u8StandardSubMenuMapping[u8LEDIndex], u16LEDBrightness);
    }

}

/*When the encoder is used for the menu, the values get out of whack. The values
 need to be reset before going back to regular operation.*/
void resetEncoderLiveInteraction(void){
    i16EncoderLiveZeroPosition = i16EncoderState;
}

void setMenuExitFlag(void){
    u8MenuExitFlag = TRUE;
}

void setMenuKeyPressFlag(void){
    u8MenuKeyPressFlag = TRUE;
}

void resetMenuParameter(void){
    switch(i8MainMenuState){
        case X_MENU:
        case Y_MENU:
        case Z_MENU:
            resetXYZParameter(i8SubMenuState);
            break;
        case RECORD_MENU:
        case OVERDUB_MENU:
        case PLAYBACK_MENU:
            resetRecParameter(i8MainMenuState-RECORD_MENU);
            break;
        case EFFECT_MENU:
            resetEffectParameter();
            break;
        case CLOCK_MENU:
            resetClockParameter();
            break;
        default:
            break;
    }
}


uint8_t initializeLoadStoreMode(void){
    setLEDState(u8LEDMainMenuMapping[i8MainMenuState], ON);
    
    int i;
    int firstPosition = FALSE;
    
    for(i=0;i<MAX_NUM_OF_LOAD_STORE_LOCATIONS;i++){
        if(getIsSequenceProgrammed(i)){
            u8LoadStoreActivePositions[i] = TRUE;
            
            /*Blink the LED of the first saved sequence if in Load mode.
             In store mode, automatically blink the LED of the first position.*/
            if(firstPosition == FALSE && i8MainMenuState == LOAD_MENU){
                firstPosition = TRUE;
                setLEDState(u8StandardSubMenuMapping[i], BLINK);
                /*Set the parameter to this LED.*/
                u8LoadStoreParameter = i;
            }
            else{
                /*Set the others solid.*/
                firstPosition = TRUE;
                setLEDState(u8StandardSubMenuMapping[i], ON);
            }

        }
        else{
            u8LoadStoreActivePositions[i] = FALSE;
            setLEDState(u8StandardSubMenuMapping[i], OFF);
        }
    }

    /*If the setting is load and there are no saved sequences, indicate an error.*/
    if(firstPosition == TRUE || (i8MainMenuState == STORE_MENU && getSequenceRecordedFlag())){
        if(i8MainMenuState == STORE_MENU){
            u8LoadStoreParameter = 0;
            setLEDState(u8StandardSubMenuMapping[0], BLINK);
        }
        setLEDState(u8LEDMainMenuMapping[i8MainMenuState], ON);
        return TRUE;
    }else{
        LEDIndicateError();
        setLEDState(u8LEDMainMenuMapping[i8MainMenuState], BLINK);
        return FALSE;
    }

}

void editLoadStoreParameter(void){
    /*If we're in Load mode,
     * only go to the LEDs that relate to stored sequences. The system will not 
     * get here if there are no saved sequences in Load mode. It will generate
     * an error and the LEDs will flash once.
     */
    int i;

    /*Turn on the LEDs for locations where there is a sequence stored.*/
    for(i=0;i<MAX_NUM_OF_LOAD_STORE_LOCATIONS;i++){
        if(u8LoadStoreActivePositions[i] == TRUE){
            setLEDState(u8StandardSubMenuMapping[i], ON);
        }
        else{
            setLEDState(u8StandardSubMenuMapping[i], OFF);
        }
    }

    /* In store mode, go to each position.
     * In load mode, only go to positions where there are stored sequences.
     *
     */
    if(i8MenuChangeFlag < 0){
        if(i8MainMenuState == STORE_MENU){
            if(--u8LoadStoreParameter > MAX_NUM_OF_LOAD_STORE_LOCATIONS){
                u8LoadStoreParameter = MAX_NUM_OF_LOAD_STORE_LOCATIONS-1;
            }
        }else{//LOAD MODE - only positions with actively stored sequences
            while(u8LoadStoreActivePositions[u8LoadStoreParameter] != TRUE){
                if(--u8LoadStoreParameter > MAX_NUM_OF_LOAD_STORE_LOCATIONS){
                    u8LoadStoreParameter = MAX_NUM_OF_LOAD_STORE_LOCATIONS-1;
                }
            }
        }
    }else{
        if(i8MainMenuState == STORE_MENU){
            if(++u8LoadStoreParameter >= MAX_NUM_OF_LOAD_STORE_LOCATIONS){
                u8LoadStoreParameter = 0;
            }
        }else{
            while(u8LoadStoreActivePositions[++u8LoadStoreParameter] != TRUE){
                if(u8LoadStoreParameter >= MAX_NUM_OF_LOAD_STORE_LOCATIONS){
                    u8LoadStoreParameter = 0;
                }
            }
        }
    }

    /*Blink the current LED */
    setLEDState(u8StandardSubMenuMapping[u8LoadStoreParameter], BLINK);
    i8MenuChangeFlag = 0;
}

void initializeRecParameterMode(uint8_t u8RecPlayOdub){
    uint8_t u8CurrentParameter;

    switch(i8SubMenuState){
        case SOURCE:
            u8CurrentParameter = getCurrentSource(u8RecPlayOdub);
            setLEDState(u8BottomParameterMapping[u8CurrentParameter], ON);
            break;
        case CONTROL:
            u8CurrentParameter = getCurrentControl(u8RecPlayOdub);
            setLEDState(u8BottomParameterMapping[u8CurrentParameter+2], ON);
            break;
        case LOOP:
            u8CurrentParameter = getCurrentLoopMode();
            setLEDState(u8BottomParameterMapping[u8CurrentParameter], ON);
            break;
        case CLK_NUMBER:
            u8CurrentParameter = getCurrentRecordClocks();
            setLEDState(u8BottomParameterMapping[u8CurrentParameter], ON);
            break;
        default:
            break;
    }

    u8MenuEntryParameter = u8CurrentParameter;
}

void editRecParameter(uint8_t u8RecPlayOdub){
    int8_t i8IncDecFlag;
    uint8_t u8CurrentParameter;

    if(i8MenuChangeFlag < 0){
        i8IncDecFlag = 0;
    }else{
        i8IncDecFlag = 1;
    }

    switch(i8SubMenuState){
        case SOURCE:
            u8CurrentParameter = getCurrentSource(u8RecPlayOdub);
            setLEDState(u8BottomParameterMapping[u8CurrentParameter], OFF);
            u8CurrentParameter ^= 1;
            setCurrentSource(u8RecPlayOdub, u8CurrentParameter);
            setLEDState(u8BottomParameterMapping[u8CurrentParameter], ON);
            break;
        case CONTROL:
            u8CurrentParameter = getCurrentControl(u8RecPlayOdub);
            setLEDState(u8BottomParameterMapping[u8CurrentParameter+2], OFF);
            if(i8MainMenuState == OVERDUB_MENU){//Presently there is no TRIGA for overdub.
               u8CurrentParameter ^= 1;
            }
            else{
               if(i8IncDecFlag){
                   if(++u8CurrentParameter >= NUM_OF_PLAYBACK_CONTROL_SETTINGS){
                       u8CurrentParameter = 0;
                   }
               }else{
                   if(--u8CurrentParameter > NUM_OF_PLAYBACK_CONTROL_SETTINGS){
                       u8CurrentParameter = NUM_OF_PLAYBACK_CONTROL_SETTINGS-1;
                   }
               }
            }
            setCurrentControl(u8RecPlayOdub, u8CurrentParameter);
            setLEDState(u8BottomParameterMapping[u8CurrentParameter+2], ON);
            break;
        case LOOP:
            u8CurrentParameter = getCurrentLoopMode();
            setLEDState(u8BottomParameterMapping[u8CurrentParameter], OFF);
            if(i8IncDecFlag){
               if(++u8CurrentParameter >= NUM_OF_LOOP_SETTINGS){
                   u8CurrentParameter = 0;
               }
           }else{
               if(--u8CurrentParameter > NUM_OF_LOOP_SETTINGS){
                   u8CurrentParameter = NUM_OF_LOOP_SETTINGS-1;
               }
           }
            setCurrentLoopMode(u8CurrentParameter);
            setLEDState(u8BottomParameterMapping[u8CurrentParameter], ON);
            break;
        case CLK_NUMBER:
            u8CurrentParameter = getCurrentRecordClocks();
            setLEDState(u8BottomParameterMapping[u8CurrentParameter], OFF);
            if(i8IncDecFlag){
               if(++u8CurrentParameter >= NUM_OF_CLOCK_SETTINGS){
                   u8CurrentParameter = 0;
               }
            }else{
               if(--u8CurrentParameter >= NUM_OF_CLOCK_SETTINGS){
                   u8CurrentParameter = NUM_OF_CLOCK_SETTINGS-1;
               }
            }
            setCurrentRecordClocks(u8CurrentParameter);
            setLEDState(u8BottomParameterMapping[u8CurrentParameter], ON);
            break;
    }

    i8MenuChangeFlag = 0;
}

void resetRecParameter(uint8_t u8RecPlayOdub){
    switch(i8SubMenuState){
        case SOURCE:
            setCurrentSource(u8RecPlayOdub, u8MenuEntryParameter);
            break;
        case CONTROL:
            setCurrentControl(u8RecPlayOdub, u8MenuEntryParameter);
            break;
        case LOOP:
            setCurrentLoopMode(u8MenuEntryParameter);
            break;
    }
}

void changeRecSubMenuState(void){
    setLEDState(u8PlaySubMenuMapping[i8SubMenuState-1], OFF);

    if(i8MenuChangeFlag < 0){
        i8SubMenuState--;
    }else{
        i8SubMenuState++;
    }

    if(i8MainMenuState != PLAYBACK_MENU){
        if(i8SubMenuState > CONTROL){
            i8SubMenuState = SOURCE;
        }
        else if(i8SubMenuState < SOURCE){
            i8SubMenuState = CONTROL;
        }
    }
    else{
        if(i8SubMenuState >= (NUM_OF_PLAYBACK_SUBMENUS+1)){
            i8SubMenuState = SOURCE;
        }
        else if(i8SubMenuState < 1){
            i8SubMenuState = LOOP;
        }
    }

    i8MenuChangeFlag = 0;
    setLEDState(u8PlaySubMenuMapping[i8SubMenuState-1], BLINK);
}

void initializeClockMode(void){
    uint8_t u8CurrentParameter;

    if(i8SubMenuState == SYNC){
        u8CurrentParameter = getCurrentGateMode();
        setLEDState(u8BottomParameterMapping[u8CurrentParameter], ON);
        u8MenuEntryParameter = u8CurrentParameter;
    }
    else{
        u8CurrentParameter = getCurrentClockMode();
        setLEDState(u8BottomParameterMapping[u8CurrentParameter], ON);
        u8MenuEntryParameter = u8CurrentParameter;
    }
}

void changeClockSubMenuState(void){
    setLEDState(u8LEDMainMenuMapping[i8SubMenuState-1], OFF);

    if(i8SubMenuState == SYNC){
        i8SubMenuState = NUMBER;
    }
    else{
        i8SubMenuState = SYNC;
    }

    setLEDState(u8LEDMainMenuMapping[i8SubMenuState-1], BLINK);

    i8MenuChangeFlag = 0;



}

void editClockParameter(void){
    uint8_t u8CurrentParameter;

    if(i8SubMenuState == NUMBER){
        u8CurrentParameter = getCurrentClockMode();
        setLEDState(u8BottomParameterMapping[u8CurrentParameter], OFF);

        if(i8MenuChangeFlag > 0){
           if(++u8CurrentParameter >= NUM_OF_CLOCK_SETTINGS){
               u8CurrentParameter = 0;
           }
        }else{
            if(--u8CurrentParameter > NUM_OF_CLOCK_SETTINGS){
                u8CurrentParameter = NUM_OF_CLOCK_SETTINGS-1;
            }
        }
        setCurrentClockMode(u8CurrentParameter);
        setLEDState(u8BottomParameterMapping[u8CurrentParameter], ON);
    }
    else{
        u8CurrentParameter = getCurrentGateMode();
        setLEDState(u8BottomParameterMapping[u8CurrentParameter], OFF);

        if(i8MenuChangeFlag > 0){
           if(++u8CurrentParameter >= NUM_OF_CLOCK_SETTINGS){
               u8CurrentParameter = 0;
           }
        }else{
            if(--u8CurrentParameter > NUM_OF_CLOCK_SETTINGS){
                u8CurrentParameter = NUM_OF_CLOCK_SETTINGS-1;
            }
        }
        setCurrentGateMode(u8CurrentParameter);
        setLEDState(u8BottomParameterMapping[u8CurrentParameter], ON);
    }

    i8MenuChangeFlag = 0;
}

void resetClockParameter(void){
    if(i8SubMenuState == NUMBER){
        setCurrentClockMode(u8MenuEntryParameter);
    }
    else{
        setCurrentGateMode(u8MenuEntryParameter);
    }
}

void initializeEffectMode(void){
    uint8_t u8CurrentParameter;

    u8CurrentParameter = getCurrentModulationMode();
    setLEDState(u8BottomParameterMapping[u8CurrentParameter], ON);
    u8MenuEntryParameter = u8CurrentParameter;
}

void editEffectParameter(void){
    uint8_t u8CurrentParameter;

    u8CurrentParameter = getCurrentModulationMode();
    setLEDState(u8BottomParameterMapping[u8CurrentParameter], OFF);

    if(i8MenuChangeFlag > 0){
       if(++u8CurrentParameter >= NUM_OF_EFFECT_SETTINGS){
           u8CurrentParameter = 0;
       }
    }else{
        if(--u8CurrentParameter > NUM_OF_EFFECT_SETTINGS){
            u8CurrentParameter = NUM_OF_EFFECT_SETTINGS-1;
        }
    }

    setCurrentModulationMode(u8CurrentParameter);
    setLEDState(u8BottomParameterMapping[u8CurrentParameter], ON);

    i8MenuChangeFlag = 0;
}

void resetEffectParameter(void){
    setCurrentModulationMode(u8MenuEntryParameter);
}

void initializeXYZParameterMode(void){
    uint16_t u16CurrentParameter;

    switch(i8SubMenuState){
        case RANGE:
            u16CurrentParameter = getCurrentRange(i8MainMenuState);
            setLEDState(u8StandardSubMenuMapping[u16CurrentParameter], ON);
            break;
        case LINEARITY:
            u16CurrentParameter = getCurrentLinearity(i8MainMenuState);
            indicateLEDContinuousXYZParameter(i8SubMenuState, u16CurrentParameter);
            break;
        case QUANTIZATION:
            u16CurrentParameter = getCurrentQuantization(i8MainMenuState);
            setLEDState(u8StandardSubMenuMapping[u16CurrentParameter], ON);
            break;
        case SLEW_RATE:
            u16CurrentParameter = getCurrentSlewRate(i8MainMenuState);
            u16CurrentParameter = MAX_SLEW_RATE - u16CurrentParameter;
            indicateLEDContinuousXYZParameter(i8SubMenuState, u16CurrentParameter);
            break;
        case TRACK_BEHAVIOR:
            u16CurrentParameter = getCurrentTrackBehavior(i8MainMenuState);
            setLEDState(u8StandardSubMenuMapping[u16CurrentParameter], ON);
            break;
        default:
            break;
    }
    u8MenuEntryParameter = u16CurrentParameter;
}

void editXYZParameter(uint8_t u8Mode){
    int8_t i8IncDecFlag;
    uint16_t u16CurrentParameter;

    if(i8MenuChangeFlag < 0){
        i8IncDecFlag = 0;
    }else{
        i8IncDecFlag = 1;;
    }

    switch(u8Mode){
        case RANGE:
            u16CurrentParameter = getCurrentRange(i8MainMenuState);
            setLEDState(u8StandardSubMenuMapping[u16CurrentParameter], OFF);
            if(i8IncDecFlag){
                if(++u16CurrentParameter > MAXIMUM_RANGE_INDEX){
                    u16CurrentParameter = 0;
                }
            }else{
                if(--u16CurrentParameter > MAXIMUM_RANGE_INDEX){
                    u16CurrentParameter = MAXIMUM_RANGE_INDEX;
                }
            }
            setLEDState(u8StandardSubMenuMapping[u16CurrentParameter], ON);
            setCurrentRange(i8MainMenuState, u16CurrentParameter);
            break;
        case LINEARITY:
            u16CurrentParameter = getCurrentLinearity(i8MainMenuState);
            if(i8IncDecFlag){
                if(++u16CurrentParameter > LINEARITY_MAX){
                    u16CurrentParameter = LINEARITY_MAX;
                }
            }else{
                if(--u16CurrentParameter > LINEARITY_MAX){
                    u16CurrentParameter = 0;
                }
            }
            indicateLEDContinuousXYZParameter(u8Mode, u16CurrentParameter);
            setCurrentLinearity(i8MainMenuState, u16CurrentParameter);
            break;
        case QUANTIZATION:
            u16CurrentParameter = getCurrentQuantization(i8MainMenuState);
            setLEDState(u8StandardSubMenuMapping[u16CurrentParameter], OFF);
            if(i8IncDecFlag){
                if(++u16CurrentParameter > MAXIMUM_QUANTIZATION_INDEX){
                    u16CurrentParameter = 0;
                }
            }else{
                if(--u16CurrentParameter > MAXIMUM_QUANTIZATION_INDEX){
                    u16CurrentParameter = MAXIMUM_QUANTIZATION_INDEX;
                }
            }
            setLEDState(u8StandardSubMenuMapping[u16CurrentParameter], ON);
            setCurrentQuantization(i8MainMenuState, u16CurrentParameter);
            break;
        case SLEW_RATE:
            u16CurrentParameter = getCurrentSlewRate(i8MainMenuState);
            //Invert the setting because it's backwards.
            //if it's 32, indicate 1024 - 32 = 982
            u16CurrentParameter = MAX_SLEW_RATE - u16CurrentParameter;
            if(i8IncDecFlag){
                u16CurrentParameter += SLEW_RATE_INCREMENT;
                if(u16CurrentParameter > MAX_SLEW_RATE){
                    u16CurrentParameter = MAX_SLEW_RATE;
                }
            }else{
                u16CurrentParameter -= SLEW_RATE_INCREMENT;
                if(--u16CurrentParameter > MAX_SLEW_RATE){
                    u16CurrentParameter = 0;
                }
            }
            indicateLEDContinuousXYZParameter(u8Mode, u16CurrentParameter);
            u16CurrentParameter = MAX_SLEW_RATE - u16CurrentParameter;
            setCurrentSlewRate(i8MainMenuState, u16CurrentParameter);
            break;
        case TRACK_BEHAVIOR:
            u16CurrentParameter = getCurrentTrackBehavior(i8MainMenuState);
            setLEDState(u8StandardSubMenuMapping[u16CurrentParameter], OFF);
            if(i8IncDecFlag){
                if(++u16CurrentParameter > MAXIMUM_TRACK_BEHAVIOR_INDEX){
                    u16CurrentParameter = 0;
                }
            }else{
                if(--u16CurrentParameter > MAXIMUM_TRACK_BEHAVIOR_INDEX){
                    u16CurrentParameter = MAXIMUM_TRACK_BEHAVIOR_INDEX;
                }
            }
            setLEDState(u8StandardSubMenuMapping[u16CurrentParameter], ON);
            setCurrentTrackBehavior(i8MainMenuState, u16CurrentParameter);
            break;
        default:
            break;
    }

    i8MenuChangeFlag = 0;
}

void resetXYZParameter(uint8_t u8subMenu){

    switch(u8subMenu){
        case RANGE:
            setCurrentRange(i8MainMenuState, u8MenuEntryParameter);
            break;
        case LINEARITY:
            setCurrentLinearity(i8MainMenuState, u8MenuEntryParameter);
            break;
        case QUANTIZATION:
            setCurrentQuantization(i8MainMenuState, u8MenuEntryParameter);
            break;
        case SLEW_RATE:
            setCurrentSlewRate(i8MainMenuState, u8MenuEntryParameter);
            break;
        case TRACK_BEHAVIOR:
            setCurrentTrackBehavior(i8MainMenuState, u8MenuEntryParameter);
            break;
        default:
            break;
    }
}
/*This function responds to encoder turning for the XYZ submenu.*/
void changeXYZSubMenuState(void){
    setLEDState(u8XYZSubMenuMapping[i8SubMenuState-1], OFF);


    if(i8MenuChangeFlag < 0){
        i8SubMenuState--;
    }else{
        i8SubMenuState++;
    }

    i8MenuChangeFlag = 0;

    if(i8SubMenuState >= (NUM_OF_XYZ_SUBMENUS+1)){
        i8SubMenuState = RANGE;
    }
    else if(i8SubMenuState < 1){
        i8SubMenuState = TRACK_BEHAVIOR;
    }

    setLEDState(u8XYZSubMenuMapping[i8SubMenuState-1], BLINK);
}

/*This function responds to encoder turning to advance the main menu*/
void changeMainMenuState(void){
    setLEDState(u8LEDMainMenuMapping[i8MainMenuState], OFF);

    if(i8MenuChangeFlag < 0){
        i8MainMenuState--;
    }else{
        i8MainMenuState++;
    }

    i8MenuChangeFlag = 0;

    if(i8MainMenuState >= NUM_OF_MAIN_MENUS){
        i8MainMenuState = X_MENU;
    }
    else if(i8MainMenuState < 0){
        i8MainMenuState = CLOCK_MENU;
    }

    setLEDState(u8LEDMainMenuMapping[i8MainMenuState], BLINK);
}

void encoderInit(void){
    u8encALastState = READ_ENCODER_A;
    u8encBLastState = READ_ENCODER_B;
}

void encoderHandler(uint16_t u16newState){
    uint8_t u8encAState,
            u8encBState;

    //Figure out which channel changed
    u8encAState = READ_ENCODER_A;
    u8encBState = READ_ENCODER_B;
    if(u8encALastState != u8encAState){//A Changed
        if(u8encALastState)
            if(u8encBState){
                i16value++;
            }
            else{
                i16value--;
            }
        else{
            if(u8encBState){
                i16value--;
            }
            else{
                i16value++;
            }
        }

        u8encALastState = u8encAState;
    }
    else{//B changed
        if(u8encBLastState)
            if(u8encAState){
                i16value--;
            }
            else{
                i16value++;
            }
        else{
            if(u8encAState){
               
                i16value++;
            }
            else{
                i16value--;
            }
        }
        u8encBLastState = u8encBState;
    }

    if((i16value%4 == 0)){
       if((i16value>>2) > i16EncoderState){
            i16EncoderState++;
            i8MenuChangeFlag = 1;
       }
       else if((i16value>>2) < i16EncoderState)
       {
           i16EncoderState--;
           i8MenuChangeFlag = -1;
       }
    }
}

