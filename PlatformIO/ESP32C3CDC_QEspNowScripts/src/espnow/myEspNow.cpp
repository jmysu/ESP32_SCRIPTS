/**
 * @file myEspNow.cpp
 * @author jimmy.su
 * @brief   EspNow related functions
 * 
 * @version 0.1
 * @date 2023-01-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "myEspNow.h"

espnow_struct _EspNowRx;

uint32_t utimeDataFly; //micros for Flying time
/**
 * @brief EspNow dataSentCB
 * 
 * @param address: mac address 
 * @param status: sent status
 *  
 */
void dataSentCB (uint8_t* address,  uint8_t status ) {
    if (ESP_NOW_SEND_SUCCESS==status) {
        if (utimeDataFly) { //Calc fly time
            utimeDataFly = micros()-utimeDataFly;
            Serial.printf ("Unicasted to *%02X%02X:%luus!\n", address[4], address[5], utimeDataFly);
            }
        else { //utimeDataFly==0; no calc fly time!
            Serial.printf ("Unicasted to *%02X%02X!\n", address[4], address[5]);
            }    
        }
    else    
        Serial.printf ("Unicast to *%02X%02X:FAIL!\n", address[4], address[5]);

    quickEspNow.onDataSent(NULL); //remove callback for reporting status    
}

/**
 * @brief EspNow dataReceivedCB
 *          
 * @param address 
 * @param data 
 * @param len 
 * @param rssi 
 * @param broadcast 
 *        _isRxInfo: to debug Rx infomation
 *        EspNow: info of rx address, msgType, rssi
 *        _EspNowMsgPackRx: data of msgPack
 */
void dataReceivedCB (uint8_t* address, uint8_t* data, uint8_t len, signed int rssi, bool broadcast) {
    if (len<=0) return;
    switch (*data) {
        case myEspNowMsgType::HELLO:
        case myEspNowMsgType::HELLO_ECHO_REQ:
            memcpy(&_EspNowRx, data, len);
            _EspNowRx.rssi = rssi;
            {
            uint32_t uOUI= address[0]<<16 | address[1]<<8 | address[2];
            uint32_t uID = address[3]<<16 | address[4]<<8 | address[5];
            _EspNowRx.macAddr64 = ((uint64_t)uOUI) << 24 | uID; //convert OUI|ID into uint64_t
            }
            //Serial.printf("RX peer:%llX time:%lu\n", EspNow.macAddr64, EspNow.ums);
            break;
        case myEspNowMsgType::HELLO_ECHO: //sendback ESPNOW-HELLO back
            memcpy(&_EspNowRx, data, len);
            _EspNowRx.rssi = rssi;
            break;
        case myEspNowMsgType::MSGPACK:
        case myEspNowMsgType::MSGPACK_ECHO_REQ:
        case myEspNowMsgType::MSGPACK_ECHO:
        case myEspNowMsgType::MSGPACK_CMD:
            _EspNowRx.msgType = *data;                     //copy msgType to EspNow
            _EspNowRx.rssi = rssi;  
            {
            uint32_t uOUI= address[0]<<16 | address[1]<<8 | address[2];
            uint32_t uID = address[3]<<16 | address[4]<<8 | address[5];
            _EspNowRx.macAddr64 = ((uint64_t)uOUI) << 24 | uID; //convert OUI|ID into uint64_t
            }                      //copy rssi to EspNow
            _EspNowMsgPackRx.unpack(data+1, len-1);       //unpack msg data
            if (_isRxInfo) {
                Serial.print("Rx msgPack:");
                for (int i=0;i<len;i++) {
                    Serial.printf(" %0X", *(data+i));    
                    }
                Serial.println();  
                //Serial.print("msgPack:");
                //for (int i=0;i<_EspNowMsgPackRx.length();i++) {
                //    Serial.printf(" %0X", _EspNowMsgPackRx.pack()[i]);    
                //    }
                //Serial.println();  
                }
            break;
            }     
    isEspNowRXed=true; 
    if (_isRxInfo) {
        Serial.printf("\n[%lu]EspNowRX: Type:0x%0X", millis(), *data);    
        Serial.printf(" Rx:%uBytes RSSI:%ddBm", len, rssi);
        Serial.printf (" From:[" MACSTR "] ", MAC2STR (address));
        Serial.printf ("%s", broadcast ? "Broadcast" : "Unicast");
        }
}

/**
 * @brief broadcastEspNowHello
 *        send HELLO to FF:FF:FF:FF:FF:FF
 * 
 */
void broadcastHello()
{ 
    espnow_struct myData;    
    myData.msgType = myEspNowMsgType::HELLO;
    myData.macAddr64 = myMac; 
    //myData.frames = counter++;
    myData.ums = millis();
    if (0==quickEspNow.send (ESPNOW_BROADCAST_ADDRESS, (uint8_t*)&myData, sizeof(espnow_struct)) ) {
        //Serial.printf ("[%lu]>>>", millis());
        //Serial.printf("ID:%0X Millis:%lu\n", myData.id, myData.ums);
        macLeader = _myNodesTime->getKey(_myNodesTime->size()-1);
        if (myData.macAddr64 == macLeader) { //Leader do 1000ms blink by himself
            uint32_t iums = myData.ums%1000;
            tickBlinkLED.once_ms(1002-iums, blinkLED); //Scheduling for next blink, with 2ms RTTI adjustment   
            }  
        else {
            //Team member following blink upon ESPNOW RX
            }    
        //Update nodes time    
        if (!_myNodesTime->has(myData.macAddr64)) {     //New member
            _myNodesAlive->put(myData.macAddr64, 0);    //Add 0 to nodesAlive
            _myNodesRSSI->put(myData.macAddr64, 0);     //Add 0 to nodesRSSI
            _myNodesTime->put(myData.macAddr64, myData.ums);
            Serial.printf("<%0llX> Added!\n", myData.macAddr64);
            //printNodesOld();
            //printNodes(); 
            }
        else { //Old member
            _myNodesTime->put(myData.macAddr64, myData.ums);
            //Update RSSI for myself
            _myNodesRSSI->put(myData.macAddr64, 0);  //No talk to myself!
            }
        } 
    else {
        Serial.printf (">>>>>>>>>> Message NOT sent\n");
        }
}

/**
 * @brief unicastHelloEchoReq
 *        request an HELLO-ECHO from mac address
 * 
 * @param mac 
 * 
 */
void unicastHelloEchoReq(uint64_t mac)
{
    espnow_struct myData;    
    myData.msgType = myEspNowMsgType::HELLO_ECHO_REQ;
    myData.macAddr64 = myMac;
    myData.ums = micros();  //Timestamp REQ started! 

    quickEspNow.onDataSent(dataSentCB); //set callback for reporting status
    //convert uint64 to mac array
    uint8_t mArray[6];
    mArray[0]=(mac>>40)&0x00FF;
    mArray[1]=(mac>>32)&0x00FF;
    mArray[2]=(mac>>24)&0x00FF;
    mArray[3]=(mac>>16)&0x00FF;
    mArray[4]=(mac>>8) &0x00FF;
    mArray[5]=mac      &0x00FF;
    
    utimeDataFly= micros(); //mark start time, 0 to disable
    if (0==quickEspNow.send (mArray, (uint8_t*)&myData, sizeof(espnow_struct)) ) {
        //Serial.printf ("[%lu]>>>", millis());
        //Serial.printf("ID:%0X Millis:%lu\n", myData.id, myData.ms);   
        //Serial.printf(">>> HELLOECHO sent!\n");   
        } 
    else {
        Serial.printf(">>> HELLOECHO NOT sent!\n");
        }

    //Serial.printf(" Size of msgType:%lu\n", sizeof(((struct espnow_struct*)0)->msgType)  ); 
    //Serial.printf(" Size of macAddr64:%lu\n", sizeof(((struct espnow_struct*)0)->macAddr64)  ); 
    //Serial.printf(" Size of dummy:%lu\n", sizeof(((struct espnow_struct*)0)->dummy)  ); 
    
}

/**
 * @brief unicastEspNowEcho()
 * 
 *        Echo back ECHOREQ to sender
 * 
 *        EspNow: EspNow data RXed
 */
void unicastHelloEcho()
{
    espnow_struct myData;    
    myData.msgType = myEspNowMsgType::HELLO_ECHO;
    myData.macAddr64 = _EspNowRx.macAddr64;
    myData.ums = _EspNowRx.ums;

    quickEspNow.onDataSent(dataSentCB); //set callback for reporting status
    //convert uint64 to mac array
    uint8_t mArray[6];
    mArray[0]=(myData.macAddr64>>40)&0x00FF;
    mArray[1]=(myData.macAddr64>>32)&0x00FF;
    mArray[2]=(myData.macAddr64>>24)&0x00FF;
    mArray[3]=(myData.macAddr64>>16)&0x00FF;
    mArray[4]=(myData.macAddr64>>8) &0x00FF;
    mArray[5]=myData.macAddr64      &0x00FF;

    utimeDataFly= micros();//mark start time, 0 to disable
    if (0==quickEspNow.send (mArray, (uint8_t*)&myData, sizeof(espnow_struct)) ) {
        //Serial.printf ("[%lu]>>>", millis());
        //Serial.printf("ID:%0X Millis:%lu\n", myData.id, myData.ms);   
        //Serial.printf(">>> HELLOECHO_ACK sent!\n");   
        } 
    else {
        Serial.printf(">>> HELLOECHO_ACK NOT sent!\n");
        }
}

/**
 * @brief unicastEspNowMsgPack   
 *        send MsgPack to mac address  
 * 
 * @param mac 
 * @param msg: variable messagePack data
 * 
 */
void unicastMsgPack(uint64_t mac, const char *msg) {
    MessagePack *EspMsgPack;
    EspMsgPack = new MessagePack();
    EspMsgPack->addText(msg);
  
    uint8_t uSize = EspMsgPack->length()+1;
    //Copy msgType+msgpack into aMsg array
    uint8_t aMsg[uSize];
    aMsg[0]=myEspNowMsgType::MSGPACK;
    memcpy(aMsg+1, EspMsgPack->pack(), EspMsgPack->length());

    quickEspNow.onDataSent(dataSentCB); //set callback for reporting status
    //convert uint64 to mac array
    uint8_t mArray[6];
    mArray[0]=(mac>>40)&0x00FF;
    mArray[1]=(mac>>32)&0x00FF;
    mArray[2]=(mac>>24)&0x00FF;
    mArray[3]=(mac>>16)&0x00FF;
    mArray[4]=(mac>>8) &0x00FF;
    mArray[5]=mac      &0x00FF;

    utimeDataFly= micros(); //mark start time, 0 to disable
    if (0==quickEspNow.send (mArray, (uint8_t*)aMsg, uSize ) ) { 
        Serial.printf(">>>>>>>>>>> MsgPack sent!\n");  
        } 
    else {
        Serial.printf (">>>>>>>>>> MsgPack NOT SENT\n");
        }  
}

/**
 * @brief unicastMsgPackEchoReq
 *        Request peer to send back msgPack!
 * 
 * @param mac 
 * @param msg variable msgPack data
 */
void unicastMsgPackEchoReq(uint64_t mac, const char *msg) {
    MessagePack *EspMsgPack;
    EspMsgPack = new MessagePack();
    EspMsgPack->addText(msg);
  
    uint8_t uSize = EspMsgPack->length()+1;
    //Copy msgType+msgpack into aMsg array
    uint8_t aMsg[uSize];
    aMsg[0]=myEspNowMsgType::MSGPACK_ECHO_REQ; //Request msgPack Echo back!
    memcpy(aMsg+1, EspMsgPack->pack(), EspMsgPack->length());

    quickEspNow.onDataSent(dataSentCB); //set callback for reporting status
    //convert uint64 to mac array
    uint8_t mArray[6];
    mArray[0]=(mac>>40)&0x00FF;
    mArray[1]=(mac>>32)&0x00FF;
    mArray[2]=(mac>>24)&0x00FF;
    mArray[3]=(mac>>16)&0x00FF;
    mArray[4]=(mac>>8) &0x00FF;
    mArray[5]=mac      &0x00FF;

    utimeDataFly= micros(); //mark start time, 0 to disable
    if (0==quickEspNow.send (mArray, (uint8_t*)aMsg, uSize ) ) { 
        Serial.printf(">>> MsgPackEchoReq sent!\n");  
        } 
    else {
        Serial.printf (">>> MsgPackEchoReq NOT SENT\n");
        }  
}

/**
 * @brief unicastMsgPackEcho
 *        echo back _EspNowMsgPackRx  
 *  
 */
void unicastMsgPackEcho()
{
    int64_t macAddr64 = _EspNowRx.macAddr64;
 
    uint8_t uSize = _EspNowMsgPackRx.length()+1;
    //Copy msgType+msgpack into aMsg array
    uint8_t aMsg[uSize];
    aMsg[0]=myEspNowMsgType::MSGPACK_ECHO; //msgPack Echo back!
    memcpy(aMsg+1, _EspNowMsgPackRx.pack(), _EspNowMsgPackRx.length());
   
    quickEspNow.onDataSent(dataSentCB); //set callback for reporting status
    //convert uint64 to mac array
    uint8_t mArray[6];
    mArray[0]=(macAddr64>>40)&0x00FF;
    mArray[1]=(macAddr64>>32)&0x00FF;
    mArray[2]=(macAddr64>>24)&0x00FF;
    mArray[3]=(macAddr64>>16)&0x00FF;
    mArray[4]=(macAddr64>>8) &0x00FF;
    mArray[5]=macAddr64      &0x00FF;

    utimeDataFly= micros();//mark start time, 0 to disable
    if (0==quickEspNow.send (mArray, (uint8_t*)&aMsg, uSize) ) {
        Serial.printf(">>> msgEcho sent!\n");
        } 
    else {
        Serial.printf(">>> msgEcho NOT sent!\n");
        }
}

void unicastMsgPackCmd(uint64_t mac, const char *cmd)
{
    MessagePack *EspMsgPack;
    EspMsgPack = new MessagePack();
    EspMsgPack->addText(cmd);
  
    uint8_t uSize = EspMsgPack->length()+1;
    //Copy msgType+msgpack into aMsg array
    uint8_t aMsg[uSize];
    aMsg[0]=myEspNowMsgType::MSGPACK_CMD; //remote msgPack cmd!
    memcpy(aMsg+1, EspMsgPack->pack(), EspMsgPack->length());

    quickEspNow.onDataSent(dataSentCB); //set callback for reporting status
    //convert uint64 to mac array
    uint8_t mArray[6];
    mArray[0]=(mac>>40)&0x00FF;
    mArray[1]=(mac>>32)&0x00FF;
    mArray[2]=(mac>>24)&0x00FF;
    mArray[3]=(mac>>16)&0x00FF;
    mArray[4]=(mac>>8) &0x00FF;
    mArray[5]=mac      &0x00FF;

    utimeDataFly= 0; //mark start time, 0 to disable
    if (0==quickEspNow.send (mArray, (uint8_t*)aMsg, uSize ) ) { 
        Serial.printf(">>> MsgPackCmd sent!\n");  
        } 
    else {
        Serial.printf (">>> MsgPackCmd NOT SENT\n");
        }     
}