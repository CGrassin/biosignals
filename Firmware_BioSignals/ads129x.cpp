#include "ads129x.h"

ADS129x::ADS129x(int cs_pin_set, int drdy_pin_set, int reset_pin_set, SPIClass* spi_set)
  : ADS1X9X(cs_pin_set, drdy_pin_set, reset_pin_set, spi_set) {}

void ADS129x::all_defaults() {
  RREGS(0, 24);  // Fetch registers

  // CONFIG1-3
  regData[ADS1X9X_REG_CONFIG1] = ADS129x_REG_CONFIG1_HIGH_RES | ADS129x_REG_CONFIG1_500SPS;
  regData[ADS1X9X_REG_CONFIG2] = ADS1X9X_REG_CONFIG2_INT_TEST;
  regData[ADS1X9X_REG_CONFIG3] = ADS129x_REG_CONFIG3_RESERVED | ADS1X9X_REG_CONFIG3_PD_REFBUF | ADS1X9X_REG_CONFIG3_BIASREF_INT | ADS1X9X_REG_CONFIG3_PD_BIAS;
  WREGS(ADS1X9X_REG_CONFIG1, 3);

  // LEAD OFF
  WREG(ADS1X9X_REG_LOFF, ADS1X9X_REG_ILEAD_OFF_6_nA | ADS1X9X_REG_FLEAD_OFF_AC_31_2HZ);

  channel_defaults();
}
void ADS129x::channel_defaults() {
  for (int i = 0; i < 8; i++)
    regData[ADS1X9X_REG_CH1SET + i] = ADS1X9X_REG_CHnSET_MUX_ELECTRODE | ADS129x_REG_CHnSET_GAIN_12;
  WREGS(ADS1X9X_REG_CH1SET, 8);
  WREG(ADS1X9X_REG_RLD_SENSP, 0xFF);
  WREG(ADS1X9X_REG_RLD_SENSN, 0xFF);
}
uint8_t ADS129x::set_sample_rate(SAMPLE_RATE sr) {
  if (regData[ADS1X9X_REG_CONFIG1] & ADS129x_REG_CONFIG1_HIGH_RES)  // For the ADS129x chips, the actual sample rate depends on the HR/LP bit
    switch (sr) {
      case SAMPLE_RATE_16000:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_16KSPS);
        break;
      case SAMPLE_RATE_8000:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_8KSPS);
        break;
      case SAMPLE_RATE_4000:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_4KSPS);
        break;
      case SAMPLE_RATE_2000:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_2KSPS);
        break;
      case SAMPLE_RATE_1000:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_1KSPS);
        break;
      case SAMPLE_RATE_500:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_500SPS);
        break;
      case SAMPLE_RATE_250:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_500SPS);
        return 2;
      case SAMPLE_RATE_125:
      default:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_500SPS);
        return 4;
    }
  else
    switch (sr) {
      case SAMPLE_RATE_16000:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_32KSPS);
        break;
      case SAMPLE_RATE_8000:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_16KSPS);
        break;
      case SAMPLE_RATE_4000:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_8KSPS);
        break;
      case SAMPLE_RATE_2000:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_4KSPS);
        break;
      case SAMPLE_RATE_1000:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_2KSPS);
        break;
      case SAMPLE_RATE_500:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_1KSPS);
        break;
      case SAMPLE_RATE_250:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_500SPS);
      case SAMPLE_RATE_125:
      default:
        this->WREG(ADS1X9X_REG_CONFIG1, this->regData[ADS1X9X_REG_CONFIG1] & ~ADS1X9X_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_500SPS);
        return 2;
    }
  return 1;
}

void ADS129x::set_channel_settings(uint8_t channelnumber, bool powerdown, uint8_t gain, INPUT_TYPE mux, bool bias, bool srb2, bool srb1) {
  uint8_t registerValue = 0;

  if (powerdown)
    registerValue |= ADS1X9X_REG_CHnSET_PD;

  switch (gain) {
    case 1:
      registerValue |= ADS129x_REG_CHnSET_GAIN_1;
      break;
    case 2:
      registerValue |= ADS129x_REG_CHnSET_GAIN_2;
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
      registerValue |= ADS129x_REG_CHnSET_GAIN_12;
      break;
    default:
      registerValue |= ADS129x_REG_CHnSET_GAIN_12;
      break;
  }

  switch (mux) {
    default:
    case INPUT_NORMAL:
      registerValue |= ADS1X9X_REG_CHnSET_MUX_ELECTRODE;
      break;
    case INPUT_SHORTED:
      registerValue |= ADS1X9X_REG_CHnSET_MUX_SHORTED;
      break;
    case INPUT_RLD:
      registerValue |= ADS1X9X_REG_CHnSET_MUX_RLD;
      break;
    case INPUT_MVDD:
      registerValue |= ADS1X9X_REG_CHnSET_MUX_MVDD;
      break;
    case INPUT_TEMP:
      registerValue |= ADS1X9X_REG_CHnSET_MUX_TEMP;
      break;
    case INPUT_TEST:
      registerValue |= ADS1X9X_REG_CHnSET_MUX_TEST;
      break;
    case INPUT_RLD_DRP:
      registerValue |= ADS1X9X_REG_CHnSET_MUX_RLD_DRP;
      break;
    case INPUT_RLD_DRN:
      registerValue |= ADS1X9X_REG_CHnSET_MUX_RLD_DRN;
      break;
  }

  if (bias) {
    WREG(ADS1X9X_REG_RLD_SENSP, this->regData[ADS1X9X_REG_RLD_SENSP] & ~(bias << channelnumber) | (bias << channelnumber));
    WREG(ADS1X9X_REG_RLD_SENSN, this->regData[ADS1X9X_REG_RLD_SENSN] & ~(bias << channelnumber) | (bias << channelnumber));
  }

  this->WREG(ADS1X9X_REG_CH1SET + channelnumber, registerValue);
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