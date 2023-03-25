#include <sys/_stdint.h>
#include "openbci.h"

OpenBCI::OpenBCI(ADS1X9X* ads, HardwareSerial* serial) {
  this->ads = ads;
  this->serial = serial;
}
void OpenBCI::startUpMessage(){
  serial->println("OpenBCI V3 8-16 channel");
  serial->print("ADS1298 Device ID: 0x");
  printHex(ads->RREG(ADS1x9x_REG_ID));
  serial->println();
  serial->println("LIS3DH Device ID: 0x00");
  serial->print("Firmware: ");
  serial->println(OPENBCI_FIRMWARE_VERSION);
  serial->println("$$$");
  serial->flush();
}
void OpenBCI::sendData(uint8_t* value) {
  serial->write((char)PCKT_START);
  serial->write(package_counter);
  for (int i = 0; i < 8 * 3; i++)
    serial->write(value[i]);
  serial->write((char)0);
  serial->write((char)0);
  serial->write((char)0);
  serial->write((char)0);
  serial->write((char)0);
  serial->write((char)0);
  serial->write((char)OPENBCI_PACKAGE_STOP_BYTE);
  //serial->flush();
  package_counter++;
}

void OpenBCI::readCMD() {
  if (serial->available()) {
    cmdBuffer[cmdIdx] = serial->read();
    cmdIdx++;

    if (cmdIdx >= OPENBCI_BUFFER_LENGTH) {  // Should never happen
      cmdIdx = 0;
    }

    if ((cmdIdx == 1 && isSingleCharCmd(cmdBuffer[0])) ||
        (cmdIdx == 2 && (cmdBuffer[0] == OPENBCI_SAMPLE_RATE_SET || cmdBuffer[0] == OPENBCI_BOARD_MODE_SET)) || 
        (cmdIdx == 9 && cmdBuffer[0] == OPENBCI_CHANNEL_CMD_SET) || 
        (cmdIdx == 5 && cmdBuffer[0] == OPENBCI_CHANNEL_IMPEDANCE_SET))
    {
      processCMD();
      cmdIdx = 0;
    }

    multiCharCmdTimeout = millis() + MULTI_CHAR_COMMAND_TIMEOUT_MS;
  }
  if (cmdIdx != 0 && millis() > multiCharCmdTimeout) {  // Should ever happen, drop cmd
    serial->print(OPENBCI_CMD_TIMEOUT_MSG);
    cmdIdx = 0;
  }
}

bool OpenBCI::isSingleCharCmd(char cmd) {
  return cmd == OPENBCI_CHANNEL_OFF_1 || cmd == OPENBCI_CHANNEL_OFF_2 || cmd == OPENBCI_CHANNEL_OFF_3 || cmd == OPENBCI_CHANNEL_OFF_4 || cmd == OPENBCI_CHANNEL_OFF_5 || cmd == OPENBCI_CHANNEL_OFF_6 || cmd == OPENBCI_CHANNEL_OFF_7 || cmd == OPENBCI_CHANNEL_OFF_8 ||
   cmd == OPENBCI_CHANNEL_OFF_9 || cmd == OPENBCI_CHANNEL_OFF_10 || cmd == OPENBCI_CHANNEL_OFF_11 || cmd == OPENBCI_CHANNEL_OFF_12 || cmd == OPENBCI_CHANNEL_OFF_13 || cmd == OPENBCI_CHANNEL_OFF_14 || cmd == OPENBCI_CHANNEL_OFF_15 || cmd == OPENBCI_CHANNEL_OFF_16 || 
   cmd == OPENBCI_CHANNEL_ON_1 || cmd == OPENBCI_CHANNEL_ON_2 || cmd == OPENBCI_CHANNEL_ON_3 || cmd == OPENBCI_CHANNEL_ON_4 || cmd == OPENBCI_CHANNEL_ON_5 || cmd == OPENBCI_CHANNEL_ON_6 || cmd == OPENBCI_CHANNEL_ON_7 || cmd == OPENBCI_CHANNEL_ON_8 ||
   cmd == OPENBCI_CHANNEL_ON_9 || cmd == OPENBCI_CHANNEL_ON_10 || cmd == OPENBCI_CHANNEL_ON_11 || cmd == OPENBCI_CHANNEL_ON_12 || cmd == OPENBCI_CHANNEL_ON_13 || cmd == OPENBCI_CHANNEL_ON_14 || cmd == OPENBCI_CHANNEL_ON_15 || cmd == OPENBCI_CHANNEL_ON_16 ||
   cmd == OPENBCI_TEST_SIGNAL_CONNECT_TO_DC || cmd == OPENBCI_TEST_SIGNAL_CONNECT_TO_GROUND || cmd == OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_FAST || cmd == OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_SLOW || cmd == OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_FAST || cmd == OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_SLOW || 
   cmd == OPENBCI_SD_LOG_FOR_HOUR_1 || cmd == OPENBCI_SD_LOG_FOR_HOUR_2 || cmd == OPENBCI_SD_LOG_FOR_HOUR_4 || cmd == OPENBCI_SD_LOG_FOR_HOUR_12 || cmd == OPENBCI_SD_LOG_FOR_HOUR_24 || cmd == OPENBCI_SD_LOG_FOR_MIN_5 || cmd == OPENBCI_SD_LOG_FOR_MIN_15 || cmd == OPENBCI_SD_LOG_FOR_MIN_30 ||  cmd == OPENBCI_SD_LOG_FOR_SEC_14 || cmd == OPENBCI_SD_LOG_STOP || 
   cmd == OPENBCI_STREAM_START || cmd == OPENBCI_STREAM_STOP || cmd == OPENBCI_MISC_QUERY_REGISTER_SETTINGS || cmd == OPENBCI_MISC_SOFT_RESET || 
   cmd == OPENBCI_CHANNEL_MAX_NUMBER_8 || cmd == OPENBCI_CHANNEL_MAX_NUMBER_16 || cmd == OPENBCI_BOARD_MODE_SET || cmd == OPENBCI_GET_VERSION ||
   cmd == OPENBCI_CHANNEL_DEFAULT_ALL_SET || cmd == OPENBCI_CHANNEL_DEFAULT_ALL_REPORT;
}

void OpenBCI::testSignals(uint8_t config2, bool shorted) {
  // Configure all channels
  for(int i = 0; i < 8; i++){
    if(shorted)
      ads->WREG(ADS1x9x_REG_CH1SET+i, (ads->regData[ADS1x9x_REG_CH1SET+i] & ~ADS1x9x_REG_CHnSET_MUX_MASK) | ADS1x9x_REG_CHnSET_MUX_SHORTED | ADS1x9x_REG_CHnSET_PD); // switch mux to test    
    else
      ads->WREG(ADS1x9x_REG_CH1SET+i, (ads->regData[ADS1x9x_REG_CH1SET+i] & ~ADS1x9x_REG_CHnSET_MUX_MASK) | ADS1x9x_REG_CHnSET_MUX_TEST | ADS1x9x_REG_CHnSET_PD); // switch mux to test    
  }
  ads->WREG(ADS1x9x_REG_CONFIG2, ADS1x9x_REG_CONFIG2_INT_TEST | config2);

  if(!ads->isContReading())
    serial->print(OPENBCI_CMD_TESTSIGNAL_SUCCESS_MSG);
}

void OpenBCI::processCMD() {
  switch (cmdBuffer[0]) {
    case OPENBCI_MISC_SOFT_RESET:
      ads->soft_reset();
      startUpMessage();
      break;
    case OPENBCI_GET_VERSION:
      serial->print(OPENBCI_FIRMWARE_VERSION);
      serial->println("$$$");
      serial->flush();
      break;

    case OPENBCI_CHANNEL_DEFAULT_ALL_SET:
      ads->channel_defaults();
      if(!ads->isContReading())
        serial->print(OPENBCI_CMD_CHANNEL_DEFAULTS_UPDATE_MSG);
      break;
    case OPENBCI_CHANNEL_DEFAULT_ALL_REPORT:
      if(!ads->isContReading())
        serial->print(OPENBCI_CMD_CHANNEL_DEFAULTS_MSG);
      break;    

    case OPENBCI_CHANNEL_OFF_1:
    case OPENBCI_CHANNEL_OFF_2:
    case OPENBCI_CHANNEL_OFF_3:
    case OPENBCI_CHANNEL_OFF_4:
    case OPENBCI_CHANNEL_OFF_5:
    case OPENBCI_CHANNEL_OFF_6:
    case OPENBCI_CHANNEL_OFF_7:
    case OPENBCI_CHANNEL_OFF_8:
      ads->switch_channel(getChannelFromCommand(cmdBuffer[0]), false);
      break;

    case OPENBCI_CHANNEL_ON_1:
    case OPENBCI_CHANNEL_ON_2:
    case OPENBCI_CHANNEL_ON_3:
    case OPENBCI_CHANNEL_ON_4:
    case OPENBCI_CHANNEL_ON_5:
    case OPENBCI_CHANNEL_ON_6:
    case OPENBCI_CHANNEL_ON_7:
    case OPENBCI_CHANNEL_ON_8:
      ads->switch_channel(getChannelFromCommand(cmdBuffer[0]), true);
      break;

    case OPENBCI_STREAM_START:
      ads->start_stream();
      break;
    case OPENBCI_STREAM_STOP:
      ads->stop_stream();
      break;

    case OPENBCI_TEST_SIGNAL_CONNECT_TO_DC:
      testSignals(ADS1x9x_REG_CONFIG2_TEST_FREQ_DC, false);
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_GROUND:
      testSignals(0,true);
      break;    
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_FAST:
      testSignals(ADS1x9x_REG_CONFIG2_TEST_FREQ_FAST, false);
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_FAST:
      testSignals(ADS1x9x_REG_CONFIG2_TEST_AMP | ADS1x9x_REG_CONFIG2_TEST_FREQ_FAST, false);
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_SLOW:
      testSignals(0, false);
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_SLOW:
      testSignals(ADS1x9x_REG_CONFIG2_TEST_AMP, false);
      break;

    case OPENBCI_CHANNEL_CMD_SET: 
      {
        uint8_t gain = 1;
        switch (cmdBuffer[3]) {
          case OPENBCI_CHANNEL_CMD_GAIN_1: gain = 1; break;
          case OPENBCI_CHANNEL_CMD_GAIN_2: gain = 2; break;
          case OPENBCI_CHANNEL_CMD_GAIN_4: gain = 4; break;
          case OPENBCI_CHANNEL_CMD_GAIN_6: gain = 6; break;
          case OPENBCI_CHANNEL_CMD_GAIN_8: gain = 8; break;
          case OPENBCI_CHANNEL_CMD_GAIN_12: gain = 12; break;
        }
        ADS1X9X::INPUT_TYPE mux = ADS1X9X::INPUT_NORMAL;
        switch (cmdBuffer[4]) {
          case OPENBCI_CHANNEL_CMD_ADC_Normal: mux = ADS1X9X::INPUT_NORMAL; break;
          case OPENBCI_CHANNEL_CMD_ADC_Shorted: mux = ADS1X9X::INPUT_SHORTED; break;
          case OPENBCI_CHANNEL_CMD_ADC_BiasMethod: mux = ADS1X9X::INPUT_RLD; break;
          case OPENBCI_CHANNEL_CMD_ADC_MVDD: mux = ADS1X9X::INPUT_MVDD; break;
          case OPENBCI_CHANNEL_CMD_ADC_Temp: mux = ADS1X9X::INPUT_TEMP; break;
          case OPENBCI_CHANNEL_CMD_ADC_TestSig: mux = ADS1X9X::INPUT_TEST; break;
          case OPENBCI_CHANNEL_CMD_ADC_BiasDRP: mux = ADS1X9X::INPUT_RLD_DRP; break;
          case OPENBCI_CHANNEL_CMD_ADC_BiasDRN: mux = ADS1X9X::INPUT_RLD_DRN; break;
        }
        ads->set_channel_settings(getChannelFromCommand(cmdBuffer[1]), 
                                  cmdBuffer[2] == OPENBCI_CHANNEL_CMD_POWER_OFF, 
                                  gain, 
                                  mux,
                                  cmdBuffer[5] == OPENBCI_CHANNEL_CMD_BIAS_INCLUDE,
                                  cmdBuffer[6] == OPENBCI_CHANNEL_CMD_SRB2_CONNECT,
                                  cmdBuffer[7] == OPENBCI_CHANNEL_CMD_SRB1_CONNECT);
      }
      break;
    case OPENBCI_SAMPLE_RATE_SET:
    {
        ADS1X9X::SAMPLE_RATE sr = ADS1X9X::SAMPLE_RATE_125;
        switch (cmdBuffer[4]) {
          case '0': sr = ADS1X9X::SAMPLE_RATE_16000; break;
          case '1': sr = ADS1X9X::SAMPLE_RATE_8000; break;
          case '2': sr = ADS1X9X::SAMPLE_RATE_4000; break;
          case '3': sr = ADS1X9X::SAMPLE_RATE_2000; break;
          case '4': sr = ADS1X9X::SAMPLE_RATE_1000; break;
          case '5': sr = ADS1X9X::SAMPLE_RATE_500; break;
          case '6': sr = ADS1X9X::SAMPLE_RATE_250; break;
          case '7': sr = ADS1X9X::SAMPLE_RATE_125; break;
          case '~': return; // FIXME
        }
        this->downsampling_factor = ads->set_sample_rate(sr);
      }
      break;
  }
}
void OpenBCI::printHex(byte _data) {
  if (_data < 0x10)
    serial->print("0");
  char buf[4];
  serial->print(itoa(_data, buf, HEX));
}
uint8_t OpenBCI::getChannelFromCommand(char asciiChar) {
  switch (asciiChar)
  {
  case OPENBCI_CHANNEL_CMD_CHANNEL_1:
  case OPENBCI_CHANNEL_ON_1:
  // case OPENBCI_CHANNEL_OFF_1:
    return 0x00;
  case OPENBCI_CHANNEL_CMD_CHANNEL_2:
  case OPENBCI_CHANNEL_ON_2:
  // case OPENBCI_CHANNEL_OFF_2:
    return 0x01;
  case OPENBCI_CHANNEL_CMD_CHANNEL_3:
  case OPENBCI_CHANNEL_ON_3:
  // case OPENBCI_CHANNEL_OFF_3:
    return 0x02;
  case OPENBCI_CHANNEL_CMD_CHANNEL_4:
  case OPENBCI_CHANNEL_ON_4:
  // case OPENBCI_CHANNEL_OFF_4:
    return 0x03;
  case OPENBCI_CHANNEL_CMD_CHANNEL_5:
  case OPENBCI_CHANNEL_ON_5:
  // case OPENBCI_CHANNEL_OFF_5:
    return 0x04;
  case OPENBCI_CHANNEL_CMD_CHANNEL_6:
  case OPENBCI_CHANNEL_ON_6:
  // case OPENBCI_CHANNEL_OFF_6:
    return 0x05;
  case OPENBCI_CHANNEL_CMD_CHANNEL_7:
  case OPENBCI_CHANNEL_ON_7:
  // case OPENBCI_CHANNEL_OFF_7:
    return 0x06;
  case OPENBCI_CHANNEL_CMD_CHANNEL_8:
  case OPENBCI_CHANNEL_ON_8:
  // case OPENBCI_CHANNEL_OFF_8:
    return 0x07;
  case OPENBCI_CHANNEL_CMD_CHANNEL_9:
  //case OPENBCI_CHANNEL_ON_9:
  case OPENBCI_CHANNEL_OFF_9:
    return 0x08;
  case OPENBCI_CHANNEL_CMD_CHANNEL_10:
  // case OPENBCI_CHANNEL_ON_10:
  case OPENBCI_CHANNEL_OFF_10:
    return 0x09;
  case OPENBCI_CHANNEL_CMD_CHANNEL_11:
  // case OPENBCI_CHANNEL_ON_11:
  case OPENBCI_CHANNEL_OFF_11:
    return 0x0A;
  case OPENBCI_CHANNEL_CMD_CHANNEL_12:
  // case OPENBCI_CHANNEL_ON_12:
  case OPENBCI_CHANNEL_OFF_12:
    return 0x0B;
  case OPENBCI_CHANNEL_CMD_CHANNEL_13:
  // case OPENBCI_CHANNEL_ON_13:
  case OPENBCI_CHANNEL_OFF_13:
    return 0x0C;
  case OPENBCI_CHANNEL_CMD_CHANNEL_14:
  // case OPENBCI_CHANNEL_ON_14:
  case OPENBCI_CHANNEL_OFF_14:
    return 0x0D;
  case OPENBCI_CHANNEL_CMD_CHANNEL_15:
  // case OPENBCI_CHANNEL_ON_15:
  case OPENBCI_CHANNEL_OFF_15:
    return 0x0E;
  case OPENBCI_CHANNEL_CMD_CHANNEL_16:
  // case OPENBCI_CHANNEL_ON_16:
  case OPENBCI_CHANNEL_OFF_16:
    return 0x0F;
  default:
    return 0x00;
  }
}