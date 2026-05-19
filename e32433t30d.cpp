#include "e32433t30d.h"

E32::E32(HardwareSerial * serial,BAUD_RATE baud, AIR_RATE air, uint8_t rx, uint8_t tx, uint8_t m0, uint8_t m1, uint8_t aux):Eserial(serial), baudRate(baud),airRate(air), rx_pin(rx), tx_pin(tx), m0_pin(m0), m1_pin(m1), aux_pin(aux){
  
  head = 0xC0;

  addH = 0xDD;
  addL = 0xcc;

  channel = 0x0B;

  parity = _8N1;

  tranPower = _30dBm;
  FEC = ON;
  wakeUp = _250;
  driveMode = RXD_PULL_UP;
  tranMode = FIXED;
}

void E32::init (){
  pinInit();
  changeMode(SLEEP);
  while(!isFree());

  Eserial->write(head);
  Eserial->write(addH);
  Eserial->write(addL);
  Eserial->write(sped);
  Eserial->write(channel);
  Eserial->write(option);

  changeMode(NORMAL);

}

void E32::init (uint8_t addr_h, uint8_t addr_l, uint8_t chan){
	
  addH = addr_h;
  addL = addr_l;

  channel = chan;
  
  pinInit();
  changeMode(SLEEP);
  while(!isFree());

  Eserial->write(head);
  Eserial->write(addH);
  Eserial->write(addL);
  Eserial->write(sped);
  Eserial->write(channel);
  Eserial->write(option);

  changeMode(NORMAL);
}

void E32::pinInit (void) const{
  pinMode(m0_pin, OUTPUT);
  pinMode(m1_pin, OUTPUT);
  pinMode(aux_pin, INPUT);

}

uint8_t E32::sendTo (uint8_t chan, uint8_t highAddr, uint8_t lowAddr, char * data, uint8_t size){
  while(!isFree()){
    ;
  }
  Eserial->write(highAddr);
  Eserial->write(lowAddr);
  Eserial->write(chan);
  for(uint8_t i=0; i<size; i++){
    Eserial->write(data[i]);
  }
  return 1;
}

uint8_t E32::sendTo (uint8_t chan, uint8_t highAddr, uint8_t lowAddr, uint8_t data){
	
  while(!isFree()){
    ;
  }
  Eserial->write(highAddr);
  Eserial->write(lowAddr);
  Eserial->write(chan);
  Eserial->write(data);
  return 1;
}

uint8_t E32::sendData (uint8_t * data, uint8_t size){
  while(!isFree()){
    ;
  }
  delay(5);
  for(uint8_t i=0; i<size; i++){
    Eserial->write(data[i]);
  }
  return 1;
}

uint8_t E32::sendByte (uint8_t  data){
	/*while(!isFree()){
    ;
  }*/
  Eserial->write(data);
  return 1;
}

uint8_t E32::available(void){
  return Eserial->available();
}

uint8_t E32::getData (void){
  return Eserial->read();
}

uint8_t E32::getParam (uint8_t * param){

  
  changeMode(SLEEP);
  
  for(uint8_t i=0; i<3; i++){
    sendByte(0xC1);
  }

  while(!available());

  uint8_t count = 0;
  while(available()){
    param[count] = getData();
    count++;
  }
  changeMode(NORMAL);

  return 1;
}

void E32::showParam (uint8_t * param){
  //head
  Serial.print("Head : ");
  Serial.println(param[0], HEX);

  //ADDH  ADDL
  Serial.print("High Address : ");
  Serial.println(param[1], HEX);
  Serial.print("Low Address : ");
  Serial.println(param[2], HEX);

  //SPED
  uint8_t pari = (param[3] & 3<<6) >> 6;
  Serial.print("UART parity bit : ");
  switch(pari){

    default:
    case _8N1:
      Serial.println("8N1 (default)");
    break;

    case _8O1:
      Serial.println("8O1");
    break;

    case _8E1:
      Serial.println("8E1");
    break;
  }

  uint8_t bp = (param[3] & 7<<3) >> 3;
  Serial.print("TTL UART baud rate (bps) : ");
  Serial.println(baud_rate[bp]);

  uint8_t air = (param[3] & 7<<0) >> 0;
  Serial.print("Air data rate (bps) : ");
  Serial.println(air_rate[air]);

  //Channel
  Serial.print("Communication channel ");
  Serial.println(param[4], HEX);

  //OPTIN
  uint8_t tran = (param[5] & 1<<7) >> 7;
  Serial.print("Fixed transmission enabling bit(similar to MODBUS) : ");
  if(tran){
    Serial.println("Fixed transmission mode");
  }else{
    Serial.println("Transparent transmission mode");
  }

  uint8_t drive = (param[5] & 1<<6) >> 6;
  Serial.print("IO drive mode (default 1) : ");
  if(drive){
    Serial.println("TXD and AUX push-pull outputs, RXD pull-up inputs");
  }else{
    Serial.println("TXD、AUX open-collector outputs, RXD open-collector inputs");
  }

  uint8_t wake = (param[5] & 7<<3) >> 3;
  Serial.print("wireless wake-up time : ");
  Serial.print((250*wake)+ 250);
  Serial.println("ms");

  uint8_t fec = (param[5] & 1<<2) >> 2;
  Serial.print("FEC switch : ");
  if(fec){
    Serial.println("Turn on FEC (default)");
  }else{
    Serial.println("Turn off FEC");
  }

  uint8_t power = (param[5] & 3) >> 0;
  Serial.print("Transmission power (approximation) : ");
  switch(power){
    case _30dBm:
      Serial.println("30dBm (default)");
    break;

    case _27dBm:
      Serial.println("27dBm");
    break;

    case _24dBm:
      Serial.println("24dBm");
    break;

    case _21dBm:
      Serial.println("21dBm");
    break;
  }

}
uint8_t E32::isFree (void){
  return digitalRead(aux_pin);
}

uint8_t E32::changeMode (MODE md){

  while(!isFree()){
    ;
  }
  delay(100);

  switch(md){
    case NORMAL:
      Eserial->flush();
      Eserial->begin(baud_rate[baudRate], SERIAL_8N1, rx_pin, tx_pin);
      digitalWrite(m0_pin, LOW);
      digitalWrite(m1_pin, LOW);
    break;

    case WAKE_UP:
      Eserial->flush();
      Eserial->begin(baud_rate[baudRate], SERIAL_8N1, rx_pin, tx_pin);
      digitalWrite(m0_pin, HIGH);
      digitalWrite(m1_pin, LOW);
    break;

    case POWER_SAVING:
      Eserial->flush();
      Eserial->begin(baud_rate[baudRate], SERIAL_8N1, rx_pin, tx_pin);
      digitalWrite(m0_pin, LOW);
      digitalWrite(m1_pin, HIGH);
    break;

    case SLEEP:
      Eserial->flush();
      Eserial->begin(9600, SERIAL_8N1, rx_pin, tx_pin);
      digitalWrite(m0_pin, HIGH);
      digitalWrite(m1_pin, HIGH);
    break;
  }
  while(!isFree());
  delay(100);

  return 1;
}


