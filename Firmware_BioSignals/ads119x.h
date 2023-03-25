/*
* This class supports: ADS1194, ADS1196, ADS1198.
* This chip is identical to ADS129X but:
* * HR/LP selection doesn't exist TODO
* * SAMPLE RATE is not the same TODO
* * The data out is 16 bits instead of 24 bits
*/
#ifndef __ADS119X_INCLUDE
#define __ADS119X_INCLUDE

#include <Arduino.h>
#include <SPI.h>
#include "ads129x.h"

class ADS119x: public ADS129X{
public:
  ADS119x(int cs_pin_set, int drdy_pin_set, int reset_pin_set, SPIClass* spi_set);
  virtual void read_data() {
    digitalWrite(cs_pin, LOW);
    for (int i = 0; i < 3; i++) 
      this->status[i] = spi->transfer(0);
    for (int i = 0; i < 8; i++){
      this->data[i*3] = spi->transfer(0);
      this->data[i*3 + 1] = spi->transfer(0);
      this->data[i*3 + 2] = 0x00; // 16 bits
    }
    digitalWrite(cs_pin, HIGH);
  }
};

#endif