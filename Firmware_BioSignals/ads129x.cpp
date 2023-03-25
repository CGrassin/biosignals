#include "ads129x.h"

ADS129x::ADS129x(int cs_pin_set, int drdy_pin_set, int reset_pin_set, SPIClass* spi_set) 
: ADS1X9X(cs_pin_set, drdy_pin_set, reset_pin_set, spi_set){}

void ADS129x::all_defaults() {
  RREGS(0,24); // Fetch registers
  
  // CONFIG1-3
  regData[ADS1x9x_REG_CONFIG1] = /*ADS129x_REG_CONFIG1_HIGH_RES |*/ ADS129x_REG_CONFIG1_500SPS;
  regData[ADS1x9x_REG_CONFIG2] = ADS129x_REG_CONFIG2_INT_TEST;
  regData[ADS1x9x_REG_CONFIG3] = ADS129x_REG_CONFIG3_RESERVED | ADS129x_REG_CONFIG3_PD_REFBUF;
  WREGS(ADS1x9x_REG_CONFIG1, 3);
  channel_defaults();
}
void ADS129x::channel_defaults(){
  for(int i = 0; i < 8; i++)
    regData[ADS1x9x_REG_CH1SET + i] = ADS1x9x_REG_CHnSET_MUX_ELECTRODE | ADS129x_REG_CHnSET_GAIN_12;
  WREGS(ADS1x9x_REG_CH1SET, 8);  
}
