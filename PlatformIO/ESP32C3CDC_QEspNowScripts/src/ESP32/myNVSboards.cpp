/**
 * @file myNVSboards.cpp
 * @author jimmy.su
 * @brief   board define NVS by platformio.ini build-flags
 * @version 0.1
 * @date 2023-01-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <Arduino.h>
#include <Preferences.h>

int8_t _pinBlink;  
bool _pinBlinkOn;
int8_t _pinNeopixel;
int8_t _pinBuzzer; 
String _BoardName;
Preferences _myPref;

void putBoardNVS()
{
    //Save NVS, once only
    _myPref.begin("my-app", false); //ReadWrite mode
    #if (C3KIT12F)
     _pinBlink   =3;
     _pinBlinkOn =1;
     _pinNeopixel=2;
     _pinBuzzer  =-1;
     #define BOARDNAME "C3KIT12F"
    #elif (C3mBUS0)
     _pinBlink   =0;
     _pinBlinkOn =0;
     _pinNeopixel=4;
     _pinBuzzer  =5; 
     #define BOARDNAME "C3mBUS0"
    #elif (C3mBUS1)
     _pinBlink   =0;
     _pinBlinkOn =0;
     _pinNeopixel=3;
     _pinBuzzer  =2; 
     #define BOARDNAME "C3mBUS1"
    #elif (C3mBUSpico)
     _pinBlink   =0;
     _pinBlinkOn =0;
     _pinNeopixel=19;
     _pinBuzzer  =18; 
     #define BOARDNAME "C3mBUSpico"
    #endif
        
    #pragma message BOARDNAME
    _myPref.putString("BoardName", BOARDNAME);
    _myPref.putChar("pinBlink",   _pinBlink);  
    _myPref.putChar("pinBlinkOn", _pinBlinkOn);
    _myPref.putChar("pinNeopixel",_pinNeopixel);
    _myPref.putChar("pinBuzzer",  _pinBuzzer);  

    //Read pinBlink from NVS::my-app
    //------------------------------
    //_myPref.begin("my-app", true); //ReadOnly mode
    _BoardName  =_myPref.getString("BoardName", "");
    _pinBlink   =_myPref.getChar("pinBlink",    -1);
    _pinBlinkOn =_myPref.getChar("pinBlinkOn",   0);
    _pinNeopixel=_myPref.getChar("pinNeopixel", -1);
    _pinBuzzer  =_myPref.getChar("pinBuzzer",   -1);

    Serial.printf("putNVS BoardName:%s\n", _BoardName);
    Serial.printf("putNVS pinBlink:%d\n",   _pinBlink);
    Serial.printf("putNVS pinBlinkOn:%d\n", _pinBlinkOn);
    Serial.printf("putNVS pinNeopixel:%d\n",_pinNeopixel);
    Serial.printf("putNVS pinBuzzer:%d\n",  _pinBuzzer);
    Serial.println();
    _myPref.end();
}

void getBoardNVS()
{
    //Read pins from NVS::my-app
    //------------------------------
    _myPref.begin("my-app", true); //ReadOnly mode
    _BoardName  =_myPref.getString("BoardName", "");
    _pinBlink   =_myPref.getChar("pinBlink",    -1);
    _pinBlinkOn =_myPref.getChar("pinBlinkOn",  0);
    _pinNeopixel=_myPref.getChar("pinNeopixel", -1);
    _pinBuzzer  =_myPref.getChar("pinBuzzer",   -1);

    Serial.printf("getNVS BoardName: %s\n", _BoardName);
    Serial.printf("getNVS pinBlink:%d\n",   _pinBlink);
    Serial.printf("getNVS pinBlinkOn:%d\n", _pinBlinkOn);
    Serial.printf("getNVS pinNeopixel:%d\n",_pinNeopixel);
    Serial.printf("getNVS pinBuzzer:%d\n",  _pinBuzzer);
    Serial.println();
    _myPref.end();
}


/*
typedef enum {
    PT_I8, PT_U8, PT_I16, PT_U16, PT_I32, PT_U32, PT_I64, PT_U64, PT_STR, PT_BLOB, PT_INVALID
} PreferenceType;
*/
signed char getPinKeyValue(const char *key)
{
    //Read key from NVS::my-app
    //------------------------------
    _myPref.begin("my-app", true); //ReadOnly mode
    
    int8_t cValue;
    if (_myPref.isKey(key)) {
        cValue   =_myPref.getChar(key, -1);
        //Serial.printf("get NVS::my-app key:%s=%d\n",   key, cValue);
        _myPref.end();
        }
    else{
        //Serial.printf("NVS::my-app key:%s not found!\n", key);
        cValue = -128;
        }
    return cValue;        
}

void putPinKeyValue(const char *key, signed char v)
{

}