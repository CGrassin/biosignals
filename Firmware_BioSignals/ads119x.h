/*
* This class supports: ADS1194, ADS1196, ADS1198.
* This chip is identical to ADS129X but:
* * HR/LP selection doesn't exist TODO
* * SAMPLE RATE is not the same
* * The data out is 16 bits instead of 24 bits
*/
#ifndef __ADS119X_INCLUDE
#define __ADS119X_INCLUDE

#include <Arduino.h>
#include <SPI.h>
#include "ads129x.h"

// ---------------------
// Register map
// Same as ADS129X
// ---------------------
// CONFIG BIT MASKS
// CONFIG 1
#define ADS119x_REG_CONFIG1_8KSPS 0b00000000
#define ADS119x_REG_CONFIG1_4KSPS 0b00000001
#define ADS119x_REG_CONFIG1_2KSPS 0b00000010
#define ADS119x_REG_CONFIG1_1KSPS 0b00000011
#define ADS119x_REG_CONFIG1_500SPS 0b00000100
#define ADS119x_REG_CONFIG1_250SPS 0b00000101
#define ADS119x_REG_CONFIG1_125SPS 0b00000110
// CONFIG 2
#define ADS119x_REG_CONFIG2_RESERVED 0b00100000  /* Reserved bits to set to 1 */
// ---------------------

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
  uint8_t set_sample_rate(SAMPLE_RATE sr){
    switch (sr) {
      case SAMPLE_RATE_16000: // This chip can't do 16kHz, set max possible sample rate instead
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS119x_REG_CONFIG1_8KSPS);
        break;
      case SAMPLE_RATE_8000:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS119x_REG_CONFIG1_8KSPS);
        break;
      case SAMPLE_RATE_4000:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS119x_REG_CONFIG1_4KSPS);
        break;
      case SAMPLE_RATE_2000:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS119x_REG_CONFIG1_2KSPS);
        break;
      case SAMPLE_RATE_1000:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS119x_REG_CONFIG1_1KSPS);
        break;
      case SAMPLE_RATE_500:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS119x_REG_CONFIG1_500SPS);
        break;
      case SAMPLE_RATE_250:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS119x_REG_CONFIG1_250SPS);
        break;
      case SAMPLE_RATE_125:
      default:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS119x_REG_CONFIG1_125SPS);
        break;
    }
    return 1;
  }
};

#endif