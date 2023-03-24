#include "ads129x.h"

ADS129x::ADS129x(int cs_pin_set, int drdy_pin_set, int reset_pin_set, SPIClass* spi_set) 
: ADS1X9X(cs_pin_set, drdy_pin_set, reset_pin_set, spi_set)
{}
void ADS129x::defaults() {
  RREGS(0,24); // Fetch registers
  
  // CONFIG1-3
  regData[ADS129x_REG_CONFIG1] = /*ADS129x_REG_CONFIG1_HIGH_RES |*/ ADS129x_REG_CONFIG1_500SPS;
  regData[ADS129x_REG_CONFIG2] = ADS129x_REG_CONFIG2_INT_TEST;
  regData[ADS129x_REG_CONFIG3] = ADS129x_REG_CONFIG3_RESERVED | ADS129x_REG_CONFIG3_PD_REFBUF;
  WREGS(ADS129x_REG_CONFIG1, 3);

  // Channels settings
  for(int i = 0; i < 8; i++)
    regData[ADS129x_REG_CH1SET + i] = ADS129x_REG_CHnSET_MUX_ELECTRODE | ADS129x_REG_CHnSET_GAIN_12;
  WREGS(ADS129x_REG_CH1SET, 8);  
}
// Helper functions
void ADS129x::read_data() {
  digitalWrite(cs_pin, LOW);
  for (int i = 0; i < 3 + 8 * 3; i++) {
    this->data[i] = spi->transfer(0);
  }
  digitalWrite(cs_pin, HIGH);
}
