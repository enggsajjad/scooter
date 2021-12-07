/**
 * \file timer.h
 *
 * \brief variable and function definitions for timings
 *
 * This is the files that contains the variables, macros, pins definitions and functions for timing related routines.
 * 
 * \date 07.09.2020
 */

/// Hardware Timer
hw_timer_t * timer = NULL;
/// Hardware timer mux initializer
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
/// Interrupt Counter
int interruptCounter;
/// Current Interrupt Counter in Sec
int currentTimeout;
/// Receive from Serial States
extern int rxState;
/// Error Indicating for AT Command Respnse
//extern byte Err;
/// Current rxState for Serial
//extern int current;

/**************************************************************************/
/*!
    @brief  ISR functions called for Reset ESP32
    @returns void
*/
/**************************************************************************/


void IRAM_ATTR resetModule() 
{
  ets_printf("reboot in 5sec...\n");
  delay(5000);
  esp_restart();
}


/**************************************************************************/
/*!
    @brief  ISR functions called Timer Elapsed
    @returns void
*/
/**************************************************************************/

void IRAM_ATTR onTimer() 
{
  portENTER_CRITICAL_ISR(&timerMux);
  //timeoutIntrupt=true;
  interruptCounter++;
  if (interruptCounter==currentTimeout)
  {
    interruptCounter=0;
    Serial.println("Timeout: "+ String(currentTimeout) +"s");
    //Err=TIMEOUT;
    //rxState = current;
    rxState = 102;
    timerAlarmDisable(timer);
  }
  portEXIT_CRITICAL_ISR(&timerMux);
}

/**************************************************************************/
/*!
    @brief  Setting up the Hardware Timer (1Sec)
    @returns void
*/
/**************************************************************************/

void setupTimer(unsigned long nSec)
{
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, nSec*1000000, true);//1sec multiply by nSec
  timerAlarmEnable(timer);
}
