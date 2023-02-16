
#include <Arduino.h>
#include <MessagePack.h>
#include "_QEspNowScript.h"

MessagePack _EspNowMsgPackRx; //MessagePack for rx

void parseMessagePack(unsigned char * data) {
    //MessagePack * message = new MessagePack();
    Serial.print("<<< MessagePack:");
    for (int i=0;i<_EspNowMsgPackRx.length();i++) {
        Serial.printf(" %02X", _EspNowMsgPackRx.pack()[i]);
        }
    Serial.println();    
    // Load the data
    //message->unpack(data, message->length());
    // Number of elements
    unsigned char count = _EspNowMsgPackRx.count();
    //Serial.print("Number of values: ");
    //Serial.println(count);
    // Walk elements
    Serial.print("("); 
    char buffer[32];
    for (unsigned char i=0; i<count; i++) {
        unsigned char type = _EspNowMsgPackRx.type(i);
        if (MESSAGE_PACK_BOOLEAN == type) {
            snprintf(buffer, sizeof(buffer), " %d", _EspNowMsgPackRx.getBoolean(i) ? 1 : 0);
        } else if (MESSAGE_PACK_SHORT == type) {
            snprintf(buffer, sizeof(buffer), " %u", _EspNowMsgPackRx.getShort(i));
        } else if (MESSAGE_PACK_INTEGER == type) {
            snprintf(buffer, sizeof(buffer), " %u", _EspNowMsgPackRx.getInteger(i));
        } else if (MESSAGE_PACK_LONG == type) {
            snprintf(buffer, sizeof(buffer), " %0X", _EspNowMsgPackRx.getLong(i));
        } else if (MESSAGE_PACK_FLOAT == type) {
            snprintf(buffer, sizeof(buffer), " %.2f", _EspNowMsgPackRx.getFloat(i));    
        } else if (MESSAGE_PACK_TEXT == type) {
            char text[128];
            _EspNowMsgPackRx.getText(i, text);
            snprintf(buffer, sizeof(buffer), " '%s'",  text);    
        }
        Serial.print(buffer);
        }
    Serial.println(" )");    
}
/*
void msgpackCommander(unsigned char * data) {
    //MessagePack * message = new MessagePack();
    Serial.print("<<< MessagePack:");
    for (int i=0;i<_EspNowMsgPackRx.length();i++) {
        Serial.printf(" %02X", _EspNowMsgPackRx.pack()[i]);
        }
    Serial.println();    
    // Load the data
    //message->unpack(data, message->length());
    // Number of elements
    unsigned char count = _EspNowMsgPackRx.count();
    //Serial.print("Number of values: ");
    //Serial.println(count);
    // Walk elements
    Serial.print("cmd:"); 
    char buffer[64];
    for (unsigned char i=0; i<count; i++) {
        unsigned char type = _EspNowMsgPackRx.type(i);
        if (MESSAGE_PACK_TEXT == type) {
            char text[128];
            _EspNowMsgPackRx.getText(i, text);
            snprintf(buffer, sizeof(buffer), " '%s'",  text);   
            cmd.feedString(String(text)); 
        }
        Serial.print(buffer);
        }
    Serial.println("");    
}
*/