#include "ads129x.h"

ADS129x::ADS129x(int cs_pin_set, int drdy_pin_set, int reset_pin_set, SPIClass* spi_set)
  : ADS1X9X(cs_pin_set, drdy_pin_set, reset_pin_set, spi_set) {}

void ADS129x::all_defaults() {
  // CONFIG1-3
  regData[ADS1X9X_REG_CONFIG1] = ADS129x_REG_CONFIG1_HIGH_RES | ADS129x_REG_CONFIG1_500SPS;
  regData[ADS1X9X_REG_CONFIG2] = ADS1X9X_REG_CONFIG2_INT_TEST;
  regData[ADS1X9X_REG_CONFIG3] = ADS129x_REG_CONFIG3_RESERVED | ADS1X9X_REG_CONFIG3_PD_REFBUF | ADS1X9X_REG_CONFIG3_BIASREF_INT | ADS1X9X_REG_CONFIG3_PD_BIAS;
  WREGS(ADS1X9X_REG_CONFIG1, 3);

  // LEAD OFF
  WREG(ADS1X9X_REG_LOFF, ADS1X9X_REG_ILEAD_OFF_6_nA | ADS1X9X_REG_FLEAD_OFF_AC_31_2HZ);
}
uint8_t ADS129x::set_sample_rate(SAMPLE_RATE sr) {
  // For the ADS129x chips, the actual sample rate depends on the HR/LP bit
  if (regData[ADS1X9X_REG_CONFIG1] & ADS129x_REG_CONFIG1_HIGH_RES) // HR mode
    switch (sr) {
      case SAMPLE_RATE_32000:
        this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS129x_REG_CONFIG1_32KSPS);
        break;
      case SAMPLE_RATE_16000:
        this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS129x_REG_CONFIG1_16KSPS);
        break;
      case SAMPLE_RATE_8000:
        this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS129x_REG_CONFIG1_8KSPS);
        break;
      case SAMPLE_RATE_4000:
        this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS129x_REG_CONFIG1_4KSPS);
        break;
      case SAMPLE_RATE_2000:
        this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS129x_REG_CONFIG1_2KSPS);
        break;
      case SAMPLE_RATE_1000:
        this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS129x_REG_CONFIG1_1KSPS);
        break;
      case SAMPLE_RATE_500:
        this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS129x_REG_CONFIG1_500SPS);
        break;
      case SAMPLE_RATE_250:
        this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS129x_REG_CONFIG1_500SPS);
        return 2;
      case SAMPLE_RATE_125:
      default:
        this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS129x_REG_CONFIG1_500SPS);
        return 4;
    }
  else // LP mode
    switch (sr) {
      case SAMPLE_RATE_32000: // This chip in LP can't do 32kHz, set max possible sample rate instead
      case SAMPLE_RATE_16000:
        this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS129x_REG_CONFIG1_32KSPS);
        break;
      case SAMPLE_RATE_8000:
        this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS129x_REG_CONFIG1_16KSPS);
        break;
      case SAMPLE_RATE_4000:
        this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS129x_REG_CONFIG1_8KSPS);
        break;
      case SAMPLE_RATE_2000:
        this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS129x_REG_CONFIG1_4KSPS);
        break;
      case SAMPLE_RATE_1000:
        this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS129x_REG_CONFIG1_2KSPS);
        break;
      case SAMPLE_RATE_500:
        this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS129x_REG_CONFIG1_1KSPS);
        break;
      case SAMPLE_RATE_250:
        this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS129x_REG_CONFIG1_500SPS);
      case SAMPLE_RATE_125:
      default:
        this->WREG(ADS1X9X_REG_CONFIG1, (this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK) | ADS129x_REG_CONFIG1_500SPS);
        return 2;
    }
  return 1;
}

uint8_t ADS129x::set_channel_settings(uint8_t channelnumber, bool powerdown, uint8_t gain, INPUT_TYPE mux, bool bias, bool srb2, bool srb1) {
  uint8_t registerValue = ADS1X9X::set_channel_settings(channelnumber, powerdown, gain, mux, bias, srb2, srb1);

  switch (gain) {
    case 1:
      registerValue |= ADS129x_REG_CHnSET_GAIN_1;
      break;
    case 2:
      registerValue |= ADS129x_REG_CHnSET_GAIN_2;
      break;
    case 3:
      registerValue |= ADS129x_REG_CHnSET_GAIN_3;
      break;
    case 4:
      registerValue |= ADS129x_REG_CHnSET_GAIN_4;
      break;
    case 6:
      registerValue |= ADS129x_REG_CHnSET_GAIN_6;
      break;
    case 8:
      registerValue |= ADS129x_REG_CHnSET_GAIN_8;
      break;
    case 12:
    default:
      registerValue |= ADS129x_REG_CHnSET_GAIN_12;
      break;
  }

  this->WREG(ADS1X9X_REG_CH1SET + channelnumber, registerValue);
  return registerValue;
}

const char * ADS129x::getRegisterName(uint8_t _address){
  switch (_address) {
    case ADS129x_REG_PACE: return "PACE";
    case ADS129x_REG_RESP: return "RESP";
    case ADS129x_REG_WCT1: return "WCT1";
    case ADS129x_REG_WCT2: return "WCT2";
    default: return ADS1X9X::getRegisterName(_address);
  }
}