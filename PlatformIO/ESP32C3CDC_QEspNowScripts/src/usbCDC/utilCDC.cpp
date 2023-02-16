#include <Arduino.h>
#include <soc/usb_serial_jtag_reg.h>

bool isUSBCDC()
  {
    uint32_t regSOF1= *((volatile uint32_t *) USB_SERIAL_JTAG_FRAM_NUM_REG);
    delay(5);
    uint32_t regSOF2= *((volatile uint32_t *) USB_SERIAL_JTAG_FRAM_NUM_REG);
    Serial.printf("USB SOF 5ms diff:%02d\n", regSOF2-regSOF1);
    
    bool isUSBCDCconnected = ((regSOF2-regSOF1)>0) ? true:false;
    if (isUSBCDCconnected) 
        log_d("USBCDC Connected!");
    else
        log_w("USBCDC NOT Connected!");

    return isUSBCDCconnected;    
  }
