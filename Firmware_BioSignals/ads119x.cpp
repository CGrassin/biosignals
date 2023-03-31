#include "ads119x.h"

ADS119X::ADS119X(int cs_pin_set, int drdy_pin_set, int reset_pin_set, SPIClass* spi_set)
  : ADS129X(cs_pin_set, drdy_pin_set, reset_pin_set, spi_set) {}

void ADS119X::all_defaults() {
  // CONFIG1-3
  regData[ADS1X9X_REG_CONFIG1] = ADS119X_REG_CONFIG1_RESERVED | ADS119X_REG_CONFIG1_250SPS;
  regData[ADS1X9X_REG_CONFIG2] = ADS119X_REG_CONFIG2_RESERVED | ADS1X9X_REG_CONFIG2_INT_TEST;
  regData[ADS1X9X_REG_CONFIG3] = ADS119X_REG_CONFIG3_RESERVED | ADS1X9X_REG_CONFIG3_PD_REFBUF | ADS1X9X_REG_CONFIG3_BIASREF_INT | ADS1X9X_REG_CONFIG3_PD_BIAS;
  WREGS(ADS1X9X_REG_CONFIG1, 3);

  // LEAD OFF
  WREG(ADS1X9X_REG_LOFF, ADS1X9X_REG_ILEAD_OFF_6_nA | ADS1X9X_REG_FLEAD_OFF_AC_31_2HZ);
}

bool ADS119X::read_data() {
  if(!digitalRead(drdy_pin)){
    digitalWrite(cs_pin, LOW);
    for (int i = 0; i < 3; i++) 
      this->status[i] = spi->transfer(0);
    for (int i = 0; i < 8; i++){
      this->data[i*3] = spi->transfer(0);
      this->data[i*3 + 1] = spi->transfer(0);
      this->data[i*3 + 2] = 0x00; // 16 bits
    }
    digitalWrite(cs_pin, HIGH);
    return true;
  }
  return false;
}
uint8_t ADS119X::set_sample_rate(SAMPLE_RATE sr){
  switch (sr) {
    case SAMPLE_RATE_32000: // This chip can't do 32kHz, set max possible sample rate instead
    case SAMPLE_RATE_16000: // This chip can't do 16kHz, set max possible sample rate instead
    case SAMPLE_RATE_8000:
      this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS119X_REG_CONFIG1_8KSPS);
      break;
    case SAMPLE_RATE_4000:
      this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS119X_REG_CONFIG1_4KSPS);
      break;
    case SAMPLE_RATE_2000:
      this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS119X_REG_CONFIG1_2KSPS);
      break;
    case SAMPLE_RATE_1000:
      this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS119X_REG_CONFIG1_1KSPS);
      break;
    case SAMPLE_RATE_500:
      this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS119X_REG_CONFIG1_500SPS);
      break;
    case SAMPLE_RATE_250:
      this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS119X_REG_CONFIG1_250SPS);
      break;
    case SAMPLE_RATE_125:
    default:
      this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS119X_REG_CONFIG1_125SPS);
      break;
  }
  return 1;
}