#include "ads1299.h"

ADS1299::ADS1299(int cs_pin_set, int drdy_pin_set, int reset_pin_set, SPIClass* spi_set)
  : ADS1X9X(cs_pin_set, drdy_pin_set, reset_pin_set, spi_set) {}

void ADS1299::all_defaults() {
  // CONFIG1-3
  regData[ADS1X9X_REG_CONFIG1] = ADS1299_REG_CONFIG1_RESERVED | ADS1299_REG_CONFIG1_250SPS;
  regData[ADS1X9X_REG_CONFIG2] = ADS1299_REG_CONFIG2_RESERVED | ADS1X9X_REG_CONFIG2_INT_TEST;
  regData[ADS1X9X_REG_CONFIG3] = ADS1299_REG_CONFIG3_RESERVED | ADS1X9X_REG_CONFIG3_PD_REFBUF | ADS1X9X_REG_CONFIG3_BIASREF_INT | ADS1X9X_REG_CONFIG3_PD_BIAS;
  WREGS(ADS1X9X_REG_CONFIG1, 3);

  // LEAD OFF
  WREG(ADS1X9X_REG_LOFF, ADS1X9X_REG_ILEAD_OFF_6_nA | ADS1X9X_REG_FLEAD_OFF_AC_31_2HZ);
}

uint8_t ADS1299::set_sample_rate(SAMPLE_RATE sr){
  switch (sr) {
    case SAMPLE_RATE_32000: // This chip can't do 32kHz, set max possible sample rate instead
    case SAMPLE_RATE_16000:
      this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS1299_REG_CONFIG1_16KSPS);
      break;
    case SAMPLE_RATE_8000:
      this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS1299_REG_CONFIG1_8KSPS);
      break;
    case SAMPLE_RATE_4000:
      this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS1299_REG_CONFIG1_4KSPS);
      break;
    case SAMPLE_RATE_2000:
      this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS1299_REG_CONFIG1_2KSPS);
      break;
    case SAMPLE_RATE_1000:
      this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS1299_REG_CONFIG1_1KSPS);
      break;
    case SAMPLE_RATE_500:
      this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS1299_REG_CONFIG1_500SPS);
      break;
    case SAMPLE_RATE_250:
      this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS1299_REG_CONFIG1_250SPS);
      break;
    case SAMPLE_RATE_125:
    default:
      this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS1299_REG_CONFIG1_250SPS);
      return 2;
      break;
  }
  return 1;
}

const char * ADS1299::getRegisterName(uint8_t _address){
  switch (_address) {
    case ADS1299_REG_MISC1: return "MISC1";
    case ADS1299_REG_MISC2: return "MISC2";
    default: return ADS1X9X::getRegisterName(_address);
  }
}

uint8_t ADS1299::set_channel_settings(uint8_t channelnumber, bool powerdown, uint8_t gain, INPUT_TYPE mux, bool bias, bool srb2, bool srb1) {
  uint8_t registerValue = ADS1X9X::set_channel_settings(channelnumber, powerdown, gain, mux, bias, srb2, srb1);

  switch (gain) {
    case 1:
      registerValue |= ADS1299_REG_CHnSET_GAIN_1;
      break;
    case 2:
      registerValue |= ADS1299_REG_CHnSET_GAIN_2;
      break;
    case 4:
      registerValue |= ADS1299_REG_CHnSET_GAIN_4;
      break;
    case 6:
      registerValue |= ADS1299_REG_CHnSET_GAIN_6;
      break;
    case 8:
      registerValue |= ADS1299_REG_CHnSET_GAIN_8;
      break;
    case 12:
      registerValue |= ADS1299_REG_CHnSET_GAIN_12;
      break;
    case 24:
    default:
      registerValue |= ADS1299_REG_CHnSET_GAIN_24;
      break;
  }

  this->WREG(ADS1X9X_REG_CH1SET + channelnumber, registerValue);
  return registerValue;
}