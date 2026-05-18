#include "Arduino.h"
#include "e32433t30d.h"

//#define TX
//#define RX

HardwareSerial uart1(1);
char num=0;

E32 myE32(&uart1, _19200, _19P2 , 27, 26, 2, 4, 5);

void setup (){
  Serial.begin(115200);
  myE32.init();
  uint8_t param[6];
  myE32.getParam(param);
  myE32.showParam(param);
  
}

void loop(){

  #ifdef TX

  myE32.sendTo(0x0B, 0xDD, 0xCC, &num,  1);
  num++;
  delay(50);
  #endif
  if(myE32.available()){
    Serial.println((int)myE32.getData());
  }

}