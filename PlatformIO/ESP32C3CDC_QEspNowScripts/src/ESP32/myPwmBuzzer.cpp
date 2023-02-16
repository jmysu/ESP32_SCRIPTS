/*
 *  PWM Buzzer
 *      ESP32 have 16 channels 
 *      ESP32-c3 have only 6 channels //#define SOC_LEDC_CHANNEL_NUM (6)
 * 
 */


#include <Arduino.h>
#include <esp32-hal-log.h>
#include <driver/ledc.h>

#include "pitches.h"
#include "themes.h"
#include "_QEspNowScript.h"

#define _duty10 256/10
#define _duty25 256/4
#define _duty50 256/2
#define _channelBuzzerPWM0 0
#define _channelBuzzerPWM1 1
bool _isTonePlaying = false;


/**
 * @brief stupPwmBuzzer
 * 
 */
void setupPwmBuzzer()
{   
static bool isPWM0BuzzerConfig;

  if (!isPWM0BuzzerConfig){
    isPWM0BuzzerConfig=true;
    if (_pinBuzzer>0) {
      //Setup PWM Channel //ledc_channel_config(&ledc_channel)
      ledcAttachPin(_pinBuzzer, _channelBuzzerPWM0); 
      //Setup PWM Freq/Timer //ledc_timer_config(&ledc_timer)
      double freq=ledcSetup(_channelBuzzerPWM0, 999, 8);  //8bit resolution
      //Setup PWM Duty
      ledcWrite(_channelBuzzerPWM0, 0); //init duty0
      log_w("PWM%d@%d:%3.0fHz", _channelBuzzerPWM0, _pinBuzzer, freq);
      }
    else {
      log_e("_pinBuzzer:%d not defined!", _pinBuzzer);
      }  
    }

    
} 

void pwmBuzzer(int8_t duty, int freq=440, int durationMills=0)
{
  setupPwmBuzzer();
  //Change Duty
  ledcWrite(_channelBuzzerPWM0, duty);
  //Change Freq
  if ((freq>9)&&(freq<10000)) ledcWriteTone(_channelBuzzerPWM0, freq);
  //Duration
  //log_d("Duty:%d Freq:%d", duty, freq);
  if (durationMills) {delay(durationMills); pwmBuzzer(0,0,0);}
}

void Beep(uint8_t duty, int duration)
{
  setupPwmBuzzer();
  _isTonePlaying = true;
  pwmBuzzer(duty, NOTE_C4, duration);
  if (duration) delay(duration);
  pwmBuzzer(0,0,0);//off
  _isTonePlaying = false; 
}

void Play_RampUp()
{
  setupPwmBuzzer();
  _isTonePlaying = true;
  for (int i=1;i<16;i++){
    pwmBuzzer(i*3, 200*i, 10); //150ms
    }
  pwmBuzzer(0,0,0);  
  _isTonePlaying = false;  
}
void Play_RampDown()
{
  setupPwmBuzzer();
  _isTonePlaying = true;
  for (int i=15;i>0;i--){
    pwmBuzzer(i*3, 200*i, 10); //150ms
    }
  pwmBuzzer(0,0,0);  
  _isTonePlaying = false;  
}
void Play_DingDong()
{
  setupPwmBuzzer();
  _isTonePlaying = true;
  pwmBuzzer(_duty50, NOTE_E5, 100);
  pwmBuzzer(0,0,100); //delay 100ms
  pwmBuzzer(_duty10, NOTE_C5, 400);
  pwmBuzzer(0,0,0); //delay 100ms
  _isTonePlaying = false;
}
void Play_DongDing()
{
  setupPwmBuzzer();
  _isTonePlaying = true;
  pwmBuzzer(_duty10, NOTE_C5, 100);
  pwmBuzzer(0,0,100); //delay 100ms
  pwmBuzzer(_duty50, NOTE_E5, 200);
  pwmBuzzer(0,0,0); //delay 100ms
  _isTonePlaying = false;
}
void Play_135()
{
  setupPwmBuzzer();
  _isTonePlaying = true;
  pwmBuzzer(_duty10, NOTE_C4, 100);
  pwmBuzzer(_duty25, NOTE_E4, 100);
  pwmBuzzer(_duty50, NOTE_G4, 300);
  pwmBuzzer(0,0,0);  
  _isTonePlaying = false;
}
void Play_531()
{
  setupPwmBuzzer();
  _isTonePlaying = true;
  pwmBuzzer(_duty50, NOTE_G4, 100);
  pwmBuzzer(_duty25, NOTE_E4, 100);
  pwmBuzzer(_duty10, NOTE_C4, 200);
  pwmBuzzer(0,0,0);  
  _isTonePlaying = false;
}
void Play_Pacman() 
{
  setupPwmBuzzer();
  _isTonePlaying = true;
  for (int thisNote = 0; thisNote < (sizeof(Pacman_note)/sizeof(int)); thisNote++) {
    int noteDuration = 1000 / Pacman_duration[thisNote]; //convert duration to time delay
    pwmBuzzer(_duty50, Pacman_note[thisNote], noteDuration*2.0);
    }
  pwmBuzzer(0,0,0);  
  _isTonePlaying = false;
}
void Play_CrazyFrog() 
{
  setupPwmBuzzer();
  _isTonePlaying = true;
  for (int thisNote = 0; thisNote < (sizeof(CrazyFrog_note)/sizeof(int)); thisNote++) {
    int noteDuration = 1000 / CrazyFrog_duration[thisNote]; //convert duration to time delay
    pwmBuzzer(_duty50, CrazyFrog_note[thisNote], noteDuration*0.5);
    }
  pwmBuzzer(0,0,0);  
  _isTonePlaying = false;
}
void Play_MarioOver() 
{
  setupPwmBuzzer();
  _isTonePlaying = true;
  for (int thisNote = 0; thisNote < (sizeof(MarioOver_note)/sizeof(int)); thisNote++) {
    int noteDuration = 1000 / MarioOver_duration[thisNote]; //convert duration to time delay
    pwmBuzzer(_duty50, MarioOver_note[thisNote], noteDuration*0.5);
    }
  pwmBuzzer(0,0,0);  
  _isTonePlaying = false;
}
void Play_MarioUW()
{
  setupPwmBuzzer();
  _isTonePlaying = true;
    for (int thisNote = 0; thisNote < (sizeof(MarioUW_note)/sizeof(int)); thisNote++) {
      int noteDuration = 1000 / MarioUW_duration[thisNote];//convert duration to time delay
      pwmBuzzer(_duty50, MarioUW_note[thisNote], noteDuration);
      }
  pwmBuzzer(0,0,0);      
  _isTonePlaying = false;    
}
void Play_Titanic()
{
  setupPwmBuzzer();
  _isTonePlaying = true;
    for (int thisNote = 0; thisNote < (sizeof(Titanic_note)/sizeof(int)); thisNote++) {
      int noteDuration = 1000 / Titanic_duration[thisNote];//convert duration to time delay
      pwmBuzzer(_duty50, Titanic_note[thisNote], noteDuration*2);
      }
  pwmBuzzer(0,0,0);  
  _isTonePlaying = false;
}
void Play_Pirates()
{ 
  setupPwmBuzzer();
  _isTonePlaying = true;
  for (int thisNote = 0; thisNote < (sizeof(Pirates_note)/sizeof(int)); thisNote++) {
    int noteDuration = 1000 / Pirates_duration[thisNote];//convert duration to time delay
    pwmBuzzer(_duty50, Pirates_note[thisNote], noteDuration);
    }
  pwmBuzzer(0,0,0);  
  _isTonePlaying = false;  
}

//for test duty  
void loopCrazyFrog515151() {
  
   Play_CrazyFrog();
   pwmBuzzer(0,0,3000);
}