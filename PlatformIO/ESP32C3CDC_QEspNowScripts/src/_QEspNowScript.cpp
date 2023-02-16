/**
 * @file _QEspNowSync.cpp
 * @author jimmy.su
 * @brief  QEspNow LED Sync
 *         Dependancy library:QuickEspNow, SimpleMap, Esp32Ticker
 * @version 0.1
 * @date 2023-01-06
 * 
 * @copyright Copyright (c) 2023
 *  
 */
#include <Arduino.h>
#include "_QEspNowScript.h"
#include "TinyJS.h"

//#if defined(BOARDNAME)
// #pragma message "Board:" BOARDNAME
//#endif

//PeerListClass myPeers;

//#define LED_ON  0
//#define LED_OFF 1
#define DEST_ADDR ESPNOW_BROADCAST_ADDRESS 

#include "espnow/myEspNow.h" //struct && enum


bool isEspNowRXed=false;        //flag for loop processing
bool _isRxInfo=false;           //for RX debug 
uint64_t myMac, macLeader;      


//-----------------------------------------------------------------------------
//#include <Ticker.h>
Ticker tickBlinkLED;
Ticker tickOffLED;
void offLED()
{
    digitalWrite(_pinBlink, !_pinBlinkOn);  //OFF LED
}
void blinkLED()
{
    digitalWrite(_pinBlink, _pinBlinkOn);   //TurnOn LED
    tickOffLED.once_ms(100, offLED);        //Off LED 100ms later
 }


//-----------------------------------------------------------------------------
Ticker tickSendHello;
void espSendHelloCB()
{
    quickEspNow.onDataSent(NULL);   //No sent callback
    broadcastHello();   
}


//extern void qjsHello();
//extern void qFoo();
//extern void setupStream();
//extern void loopStream();
//extern void readCmdFile();
//=============================================================================
void setup () {
    //pinMode(_pinBlink, OUTPUT);
    
    #if (ARDUINO_USB_CDC_ON_BOOT)
     log_w("USB_CDC_ON_BOOT enabled!");
     if (isUSBCDC()) {
       if (!Serial) {Serial.begin(115200); delay(600);}
       // Workaround for when USB CDC is unplugged #7583 
       Serial.setTxTimeoutMs(1);   // <<<====== solves USBCDC issue of delay and ANSI serial corruption
       }
    #else
     log_w("USB_CDC_ON_BOOT disabled!");
     if (!Serial) {Serial.begin(115200); delay(300);}       
    #endif

    printESP32info();
    getBoardNVS();
    if (_BoardName==""){ //Didn't find BoardName in NVS!
        Serial.printf("BoardNVS not found! Writing BoardNVS:%s...\n!", _BoardName);
        putBoardNVS();
        }
    if (_pinBlink>=0)
        pinMode(_pinBlink, OUTPUT);

    //Save pinBlink to NVS::my-app
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect(false);

    //myId=getESP32ChipID24(); //get my ID
    //myMac = ESP.getEfuseMac();
    //Serial.printf("EfuseMAC:%012X", myMac);
    uint8_t mac[6];
    WiFi.macAddress(mac);
    Serial.printf("WiFiMAC:" MACSTR "\n", MAC2STR(mac));
    uint32_t uOui = mac[0]<<16 | mac[1]<<8 | mac[2];
    uint32_t uId  = mac[3]<<16 | mac[4]<<8 | mac[5];
    myMac = ((uint64_t)uOui)<<24 | uId;
    macLeader = myMac;
    //Serial.printf("uOui Id:%X:%X", uOui, uId);
    Serial.printf("My MAC address:%llX\n", myMac);
    

    //Setup Nodes Maps---------------------
    setupNodesMap();
    _myNodesAlive->clear();
    _myNodesTime->put(myMac, millis());
    printNodesAlive();
    //=====================================
    //quickEspNow.onDataSent(dataSentCB);
    quickEspNow.onDataRcvd(dataReceivedCB);
    quickEspNow.begin(1); // If you are not connected to WiFi, channel should be specified
    tickSendHello.attach_ms(1000, espSendHelloCB); //Send HELLO every 1000ms

    showBanner();

    TinyScript.begin(2048);
}

uint32_t lastStatusMs=0; 
uint8_t counter=0;

void loop () {
    //print Nodes & check KeepAlive every 3seconds
    if ((millis()-lastStatusMs)>=3000) {
        lastStatusMs = millis(); //update lastStatus counter
        //printNodesOld();
        //printNodesAlive();
        //Check KeepAlive, drop node when timeDiff<1000ms
        for (int i=0;i<_myNodesTime->size();i++) { 
            uint64_t mac=_myNodesTime->getKey(i); //get key
            uint32_t timeOld = _myNodesAlive->getData(_myNodesAlive->getIndex(mac)); //find key/value in alive nodes
            uint32_t timeNew = _myNodesTime->getData(i);
            uint32_t timeDiff= timeNew - timeOld;
           
            _myNodesAlive->put(mac, timeNew); //update NodesAlive w/ new time
            //Serial.printf(" %0llX:%lu",  mac, timeDiff);
            if (timeDiff<1000){
                _myNodesTime->remove(i);
                _myNodesAlive->remove(i);
                Serial.printf(" <%0llX> Dropped!\n", mac);
                }
            }   
        //printNodesRSSI();    
        }
    if (isEspNowRXed) { //Got _EspNowRx RX data?
        isEspNowRXed=false;
        switch (_EspNowRx.msgType) {
            case myEspNowMsgType::HELLO: 
                //idLeader is the last one in _myNodesTime
                macLeader = _myNodesTime->getKey(_myNodesTime->size()-1);
                if (_EspNowRx.macAddr64 == macLeader) { //Leader's HELLO
                    tickBlinkLED.once_ms(1000-(_EspNowRx.ums%1000), blinkLED); //Scheduling for next blink and try to sync w/ Leader   
                    }
                if (!_myNodesTime->has(_EspNowRx.macAddr64)) {     //New memeber
                    _myNodesAlive->put(_EspNowRx.macAddr64, 0);    //Add 0 to nodesAlive
                    _myNodesRSSI->put(_EspNowRx.macAddr64, 0);     //Add 0 to nodesRSSI
                    _myNodesTime->put(_EspNowRx.macAddr64, _EspNowRx.ums);
                    Serial.printf("[%04lu] <%0llX> Added!\n", millis(), _EspNowRx.macAddr64);
                    //printNodesOld();
                    //printNodes();
                    }
                else  {  //Old member
                    _myNodesTime->put(_EspNowRx.macAddr64, _EspNowRx.ums);  
                    //Average RSSI for node
                    int8_t rssiOld = _myNodesRSSI->getData(_myNodesRSSI->getIndex(_EspNowRx.macAddr64));
                    //Serial.printf("<%0llX>RSSI:%d/%d\n",  EspNow.macAddr64, rssiOld, EspNow.rssi);
                    _myNodesRSSI->put(_EspNowRx.macAddr64, (_EspNowRx.rssi/2 + rssiOld/2));
                    }
                break; 
            case myEspNowMsgType::HELLO_ECHO_REQ:   //Got an Hello echo request
                unicastHelloEcho();                 //Echo the REQ, send back HELLO
                break;
            case myEspNowMsgType::HELLO_ECHO:       //Echo acked! calculate RTTI 
                //Serial.printf("myMac:%0llX peerMac:%0llX\n", myMac, _EspNowRx.macAddr64);
                if (myMac == _EspNowRx.macAddr64) {
                    uint32_t uRTTI = micros() - _EspNowRx.ums; //_EspNowRx.ums is the timestamp REQ fired!
                    Serial.printf("!!! ECHO RTTI=%luus from:%llX (%d)dBm\n", uRTTI, _EspNowRx.macAddr64, _EspNowRx.rssi);
                    //Serial.printf(" From mac:%02x:%02x:%02x:%02x:%02x:%02x\n", MAC2STR(EspNow.mac));
                    }
                break;           
            case myEspNowMsgType::MSGPACK:      //Got msgPack
            case myEspNowMsgType::MSGPACK_ECHO: //Got msgPack echo
                parseMessagePack(_EspNowMsgPackRx.pack());
                break; 
            case myEspNowMsgType::MSGPACK_ECHO_REQ: //Peer request to send back msgPack
                unicastMsgPackEcho();
                break;    
            case myEspNowMsgType::MSGPACK_CMD: //Got msgPack to Commander cmd
                //msgpackCommander(_EspNowMsgPackRx.pack());
                break;    
                }    
            }

        if (eSCRIPT::TINY_SCRIPT == iMyScriptType) {
            TinyScript.pollREPL();    
            }
        else if (eSCRIPT::TINY_BASICPLUS == iMyScriptType) {
            while (Serial.available()) {       
                loopTinyBasicPlus(); 
                }
            }
        else {//TinyJS
            //loopTinyJS();
            }
}