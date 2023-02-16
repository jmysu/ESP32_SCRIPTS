/**
 * Send CQ (-.-. --.-) on the status LED.
 *
 * Demonstrate the most simple usage: send one Morse Code message, once,
 * while blocking (not doing anything else).
 */
#include <Arduino.h>
#include <morse.h>
#include "_QEspNowScript.h"


SpeakerMorseSender cqSender(
    _pinBuzzer,
    880, // tone frequency 'A'
    220,   // default; No carrier frequency
    20.0); // default; 12 wpm

void sendMorseCodes()
{
    setupPwmBuzzer();
    cqSender.setup();
    String sBoard = String("EspNowCmdr");
    cqSender.setMessage(String("cq cq cq de ") + sBoard + String(" k   "));
    
    //_isTonePlaying = true;
    cqSender.sendBlocking();
    //_isTonePlaying = false;
}

void sendMorseCodes(char *msg)
{
    setupPwmBuzzer();
    log_d("CQ Sending message...[%s]", msg);
    cqSender.setup();
    cqSender.setMessage(String(msg));

    //_isTonePlaying = true;
    cqSender.sendBlocking();
    //_isTonePlaying = false;
}
/*
void loopMorseCodeSending()
{
    if (!cqSender.continueSending()){
        cqSender.startSending();
        }
}
*/
