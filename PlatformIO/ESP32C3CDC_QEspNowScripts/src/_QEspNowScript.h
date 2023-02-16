#ifndef _QESPNOWCMDR_H
#define _QESPNOWCMDR_H
#include <Arduino.h>
 
#include <tinyscript.h>
enum eSCRIPT{TINY_SCRIPT = 0, TINY_BASICPLUS, TINY_JS};
extern int iMyScriptType;

//TinyBasicPlus
#include <myTinyBasicPlus.h>
#include <sys/stat.h>


//TinyJS
extern void myTinyJS();

#include <WiFi.h>
#include <esp_wifi.h>
#include <TimeLib.h>

#include <QuickEspNow.h>
#include "espnow/myEspNow.h"
//ESPNOW



//Firmware Version Major.mid.minor
#define FW_MAJOR 0
#define FW_MID   9
#define FW_MINOR "0211"

#if CONFIG_ARDUHAL_LOG_COLORS
#define _BLK "\e[0;30m"
#define _red "\e[0;31m"
#define _RED "\e[1;31m"
#define _grn "\e[0;32m"
#define _GRN "\e[1;32m"
#define _YEL "\e[0;33m"
#define _blu "\e[0;34m"
#define _BLU "\e[1;34m"
#define _MAG "\e[0;35m"
#define _CYN "\e[0;36m"
#define _WHT "\e[0;37m"
#endif
extern void showBanner();

//NVS
#include <Preferences.h>
extern int8_t _pinBlink;  
extern bool _pinBlinkOn; 
extern int8_t _pinNeopixel;
extern int8_t _pinBuzzer; 
extern String _BoardName;
extern Preferences _myPref;
extern void putBoardNVS();
extern void getBoardNVS();
extern signed char getPinKeyValue(const char *key);
extern void putPinKeyValue(const char *key, signed char v);

//Commander
//extern void setupCmdSerial();
//extern Commander cmd;
//extern void msgpackCommander(unsigned char * data);

//ESP32
extern bool isUSBCDC();
extern uint32_t getESP32ChipID24();
extern void neopixelRGB(uint8_t r, uint8_t g, uint8_t b);
extern void neopixelSetupBlink(int msBlink);
extern void printESP32info();
extern float readTempSensor();
extern void setupTempSensor();

//Neopixel
extern int _msBlinkInterval; 
extern uint8_t _neoR, _neoG, _neoB;
extern void neopixelRGB(uint8_t r, uint8_t g, uint8_t b);
extern void neopixelSetupBlink(int msBlink);

//Buzzer
extern void setupPwmBuzzer();
extern void pwmBuzzer(int8_t duty, int freq, int durationMills);
extern void Play_DingDong();
extern void Play_DongDing();
extern void Play_Pacman();
extern void Play_CrazyFrog();
extern void Play_Titanic();
extern void Play_Pirates();
extern void Play_MarioUW();
extern void Play_MarioOver(); 
extern void Play_135();
extern void Play_531();
extern void Beep(uint8_t duty, int duration);
extern void Play_RampUp();
extern void Play_RampDown();
extern void sendMorseCodes();
extern void sendMorseCodes(char *msg);
extern void setupEspPWMBuzzer(int ms);

//FS
extern void setupListFS();
extern void LittleFSlistFS();
extern String readScriptFile(const char* path);

//WiFi ralated
extern uint8_t* getPreferencesWiFiSSID();
extern uint8_t* getPreferencesWiFiPASS(); 
extern void putPreferencesWiFiSSID(uint8_t* ptrSSID);
extern void putPreferencesWiFiPASS(uint8_t* ptrPASS);
extern void wifiScan();
extern void wifiScanHomeMobile();
extern void syncSNTP();

//Nodes related
extern void printNodes();
extern void printNodesAlive();
extern void printNodesRSSI();

#endif
