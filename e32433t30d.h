#ifndef E32433T30D_H
#define E32433T30D_H

#include "Arduino.h"

enum BAUD_RATE{_1200=0, _2400, _4800, _9600, _19200, _38400, _57600, _115200 };
enum WAKE_UP{_250=0, _500, _750,_1000, _1250, _1500, _1750, _2000};
enum MODE{NORMAL=0, WAKE_UP, POWER_SAVING, SLEEP};
enum AIR_RATE{_OP3=0, _1P3, _2P4, _4P8, _9P6, _19P2};//_0P3 = 0.3, _19P2 = 19.2
enum PARITY{_8N1=0, _8O1, _8E1, };
enum TRANSMISSION_MODE{TRANSPARENT=0, FIXED};
enum IO_DRIVE{RXD_OPEN_COLLECTOR=0, RXD_PULL_UP};
enum TRAN_POWER{_30dBm=0, _27dBm, _24dBm, _21dBm};
enum FEC_SWITCH{OFF=0, ON};

class E32{
  private:
  const uint32_t baud_rate[8] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
  const uint16_t air_rate[8]={300, 1200, 2400, 4800, 9600, 19200, 19200, 19200 };
  HardwareSerial * Eserial;

  //e32 pin
  uint8_t m0_pin;
  uint8_t m1_pin;
  uint8_t aux_pin;
  uint8_t rx_pin;
  uint8_t tx_pin;

  //Address
  uint8_t addH;
  uint8_t addL;
  uint8_t channel;

  //Instruction Sequence
  //Header
  uint8_t head;
  
  //SPED
  union{
    uint8_t sped;
    struct{
      uint8_t airRate:3;
      uint8_t baudRate:3;
      uint8_t parity:2;
    };
  };

  union{
    uint8_t option;
    struct{
      uint8_t tranPower:2;
      uint8_t FEC:1;
      uint8_t wakeUp:3;
      uint8_t driveMode:1;
      uint8_t tranMode:1;
    };
  };

  public:

  E32(HardwareSerial * serial,BAUD_RATE baud, AIR_RATE air, uint8_t rx, uint8_t tx, uint8_t m0, uint8_t m1, uint8_t aux);
  void init ();
  void pinInit (void) const;
  uint8_t sendData (uint8_t * data, uint8_t size);
  uint8_t sendByte (uint8_t  data);
  uint8_t available(void);
  uint8_t getParam (uint8_t * param);
  uint8_t getData (void);
  void showParam (uint8_t * param);
  uint8_t isFree (void);
  uint8_t changeMode (MODE md);
  uint8_t sendTo (uint8_t chan, uint8_t highAddr, uint8_t lowAddr, char * data, uint8_t size);
  uint8_t sendTo (uint8_t chan, uint8_t highAddr, uint8_t lowAddr, uint8_t data);
};

#endif