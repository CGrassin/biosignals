/*
* This class supports: ADS1194, ADS1196, ADS1198.
* This chip is identical to ADS129X but:
* * HR/LP selection doesn't exist
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
#define ADS119x_REG_CONFIG1_RESERVED 0b00000000
#define ADS119x_REG_CONFIG1_8KSPS 0b00000000
#define ADS119x_REG_CONFIG1_4KSPS 0b00000001
#define ADS119x_REG_CONFIG1_2KSPS 0b00000010
#define ADS119x_REG_CONFIG1_1KSPS 0b00000011
#define ADS119x_REG_CONFIG1_500SPS 0b00000100
#define ADS119x_REG_CONFIG1_250SPS 0b00000101
#define ADS119x_REG_CONFIG1_125SPS 0b00000110
// CONFIG 2
#define ADS119x_REG_CONFIG2_RESERVED 0b00100000  /* Reserved bits to set to 1 */
// CONFIG 3
#define ADS119x_REG_CONFIG3_RESERVED 0b01000000  /* Reserved bits to set to 1 */
// ---------------------

class ADS119x: public ADS129x{
public:
  ADS119x(int cs_pin_set, int drdy_pin_set, int reset_pin_set, SPIClass* spi_set);
  virtual void all_defaults();
  virtual bool read_data();
  virtual uint8_t set_sample_rate(SAMPLE_RATE sr);
  // set_channel_settings and getRegisterName are the same as ADS129X
};

#endif