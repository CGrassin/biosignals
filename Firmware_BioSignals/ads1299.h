/*
* This class supports: ADS1299.
*/
#ifndef __ADS1299_INCLUDE
#define __ADS1299_INCLUDE

#include <Arduino.h>
#include <SPI.h>
#include "ads1x9x.h"

// ---------------------
// Register map
#define ADS1299_REG_MISC1 0x15
#define ADS1299_REG_MISC2 0x16
// 0x18 and 0x19 don't exist
// ---------------------
// CONFIG BIT MASKS
// CONFIG 1
#define ADS1299_REG_CONFIG1_RESERVED 0b10010000  /* Reserved bits to set to 1 */
#define ADS1299_REG_CONFIG1_16KSPS 0b00000000
#define ADS1299_REG_CONFIG1_8KSPS 0b00000001
#define ADS1299_REG_CONFIG1_4KSPS 0b00000010
#define ADS1299_REG_CONFIG1_2KSPS 0b00000011
#define ADS1299_REG_CONFIG1_1KSPS 0b00000100
#define ADS1299_REG_CONFIG1_500SPS 0b00000101
#define ADS1299_REG_CONFIG1_250SPS 0b00000110
// CONFIG 2
#define ADS1299_REG_CONFIG2_RESERVED 0b11000000  /* Reserved bits to set to 1 */
// CONFIG 3
#define ADS1299_REG_CONFIG3_RESERVED 0b01100000  /* Reserved bits to set to 1 */
// Individual Channel Settings
#define ADS1299_REG_CHnSET_GAIN_1 0b00000000
#define ADS1299_REG_CHnSET_GAIN_2 0b00010000
#define ADS1299_REG_CHnSET_GAIN_4 0b00100000
#define ADS1299_REG_CHnSET_GAIN_6 0b00110000
#define ADS1299_REG_CHnSET_GAIN_8 0b01000000
#define ADS1299_REG_CHnSET_GAIN_12 0b01010000
#define ADS1299_REG_CHnSET_GAIN_24 0b01100000
#define ADS1299_REG_CHnSET_SRB2    0b00001000
// MISC1
#define ADS1299_REG_MISC1_SRB1    0b00100000
// ---------------------

class ADS1299: public ADS1X9X{
public:
  ADS1299(int cs_pin_set, int drdy_pin_set, int reset_pin_set, SPIClass* spi_set);
  void all_defaults();
  uint8_t set_sample_rate(SAMPLE_RATE sr);
  virtual uint8_t set_channel_settings(uint8_t channelnumber, bool powerdown, uint8_t gain, INPUT_TYPE mux, bool bias, bool srb2, bool srb1);
  const char * getRegisterName(uint8_t _address);
};
#endif
