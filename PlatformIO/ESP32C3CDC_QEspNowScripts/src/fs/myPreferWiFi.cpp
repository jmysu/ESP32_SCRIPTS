/*
   This example shows how to use Preferences (nvs) to store a
structure.  Note that the maximum size of a putBytes is 496K
or 97% of the nvs partition size.  nvs has signifcant overhead,
so should not be used for data that will change often.
*/ 
#include <Arduino.h>
#include "_QEspNowScript.h" 

#include <Preferences.h>
Preferences prefs;

typedef struct {
  uint8_t cSSID[16];
  uint8_t cPASS[16];
  uint8_t setting1;
  uint8_t setting2;
} myprefs_t;
myprefs_t myNVS;

void putPreferencesWiFiSSID(uint8_t* ptrSSID)
{
  prefs.begin("myprefs"); // use "myprefs" namespace
  size_t prefsLen = prefs.getBytesLength("myprefs");
  prefs.getBytes("myprefs", &myNVS, prefsLen);
  if (prefsLen % sizeof(myprefs_t)) { // simple check that data fits
    log_e("Data is not correct size!");
    }
  sprintf((char*)myNVS.cSSID, "%s", (char*)ptrSSID);
  prefs.putBytes("myprefs", &myNVS, sizeof(myprefs_t));   
  prefs.end(); 
}
void putPreferencesWiFiPASS(uint8_t* ptrPASS)
{
  prefs.begin("myprefs"); // use "myprefs" namespace
  size_t prefsLen = prefs.getBytesLength("myprefs");
  prefs.getBytes("myprefs", &myNVS, prefsLen);
  if (prefsLen % sizeof(myprefs_t)) { // simple check that data fits
    log_e("Data is not correct size!");
    }
  sprintf((char*)myNVS.cPASS, "%s", (char*)ptrPASS);
  prefs.putBytes("myprefs", &myNVS, sizeof(myprefs_t));   
  prefs.end();  
}

uint8_t* getPreferencesWiFiSSID()
{
 prefs.begin("myprefs"); // use "myprefs" namespace
 size_t prefsLen = prefs.getBytesLength("myprefs");
 prefs.getBytes("myprefs", &myNVS, prefsLen);
 if (prefsLen % sizeof(myprefs_t)) { // simple check that data fits
    log_e("Preference Data is not correct size!");
    const char* errMorse= "sos ssid";
    sendMorseCodes((char*)errMorse);
    }
 prefs.end();    
 return myNVS.cSSID;
}

uint8_t* getPreferencesWiFiPASS()
{
 prefs.begin("myprefs"); // use "myprefs" namespace
 size_t prefsLen = prefs.getBytesLength("myprefs");
 prefs.getBytes("myprefs", &myNVS, prefsLen);
 if (prefsLen % sizeof(myprefs_t)) { // simple check that data fits
    log_e("Data is not correct size!");
    const char* errMorse= "sos pass";
    sendMorseCodes((char*)errMorse);
    }
 prefs.end();    
 return myNVS.cPASS;
}

void savePreferencesWiFi() {
  char buffer[64];

  prefs.begin("myprefs"); // use "myprefs" namespace
  size_t prefsLen = prefs.getBytesLength("myprefs");
  if (prefsLen == 0 ) {//Write when no data there
    String sSSID = "Breezexxxxxxx";
    String sPASS = "0932xxxxxx";
    memcpy(myNVS.cSSID, sSSID.c_str(), sSSID.length());
    memcpy(myNVS.cPASS, sPASS.c_str(), sPASS.length());
    myNVS.setting1= 0x55;
    myNVS.setting2= 0xAA;
    prefs.putBytes("myprefs", &myNVS, sizeof(myprefs_t));
    }

  prefsLen = prefs.getBytesLength("myprefs");
  prefs.getBytes("myprefs", buffer, prefsLen);
  if (prefsLen % sizeof(myprefs_t)) { // simple check that data fits
    log_e("Data is not correct size!");
    return;
  }
  myprefs_t *data = (myprefs_t *) buffer; // cast the bytes into a struct ptr
  Serial.println((char*)data[0].cSSID);
  Serial.println((char*)data[0].cPASS);
  Serial.println(data[0].setting1);
  Serial.println(data[0].setting2);
  prefs.end(); 
}