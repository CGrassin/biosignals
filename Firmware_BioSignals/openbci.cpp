#include <sys/_stdint.h>
#include "openbci.h"

OpenBCI::OpenBCI(ADS129x* ads, HardwareSerial* serial) {
  this->ads = ads;
  this->serial = serial;
}
void OpenBCI::startUpMessage(){
  serial->println("OpenBCI V3 8-16 channel");
  serial->println("ADS1298 Device ID: 0x00");
  serial->println("LIS3DH Device ID: 0x00");
  serial->print("Firmware: ");
  serial->println(OPENBCI_FIRMWARE_VERSION);
  serial->println("$$$");
  serial->flush();
}
void OpenBCI::sendData(uint8_t* value) {
  serial->write((char)PCKT_START);
  serial->write(package_counter);
  for (int i = 3; i < 3 + 8 * 3; i++)
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

void OpenBCI::testSignals(uint8_t config2) {
  // Switch all channel on
  for(int i = 0; i < 8; i++){
    ads->switch_channel(i, true); // turn on
    ads->WREG(ADS129x_REG_CH1SET+i, ads->regData[ADS129x_REG_CH1SET+i] & ~ADS129x_REG_CHnSET_MUX_MASK | ADS129x_REG_CHnSET_MUX_TEST); // switch mux to test    
  }
  ads->WREG(ADS129x_REG_CONFIG2, ADS129x_REG_CONFIG2_INT_TEST | config2);
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
      ads->switch_channel(0, false);
      break;
    case OPENBCI_CHANNEL_OFF_2:
      ads->switch_channel(1, false);
      break;
    case OPENBCI_CHANNEL_OFF_3:
      ads->switch_channel(2, false);
      break;
    case OPENBCI_CHANNEL_OFF_4:
      ads->switch_channel(3, false);
      break;
    case OPENBCI_CHANNEL_OFF_5:
      ads->switch_channel(4, false);
      break;
    case OPENBCI_CHANNEL_OFF_6:
      ads->switch_channel(5, false);
      break;
    case OPENBCI_CHANNEL_OFF_7:
      ads->switch_channel(6, false);
      break;
    case OPENBCI_CHANNEL_OFF_8:
      ads->switch_channel(7, false);
      break;

    case OPENBCI_CHANNEL_ON_1:
      ads->switch_channel(0, true);
      break;
    case OPENBCI_CHANNEL_ON_2:
      ads->switch_channel(1, true);
      break;
    case OPENBCI_CHANNEL_ON_3:
      ads->switch_channel(2, true);
      break;
    case OPENBCI_CHANNEL_ON_4:
      ads->switch_channel(3, true);
      break;
    case OPENBCI_CHANNEL_ON_5:
      ads->switch_channel(4, true);
      break;
    case OPENBCI_CHANNEL_ON_6:
      ads->switch_channel(5, true);
      break;
    case OPENBCI_CHANNEL_ON_7:
      ads->switch_channel(6, true);
      break;
    case OPENBCI_CHANNEL_ON_8:
      ads->switch_channel(7, true);
      break;

    case OPENBCI_STREAM_START:
      ads->start_stream();
      break;
    case OPENBCI_STREAM_STOP:
      ads->stop_stream();
      break;

    case OPENBCI_TEST_SIGNAL_CONNECT_TO_DC:
      testSignals(ADS129x_REG_CONFIG2_TEST_FREQ_DC);
      break;
    // case OPENBCI_TEST_SIGNAL_CONNECT_TO_GROUND:
    //   testSignals(ADS129x_REG_CONFIG2_TEST_FREQ_DC);
    //   break;    
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_FAST:
      testSignals(ADS129x_REG_CONFIG2_TEST_FREQ_FAST);
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_FAST:
      testSignals(ADS129x_REG_CONFIG2_TEST_AMP | ADS129x_REG_CONFIG2_TEST_FREQ_FAST);
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_SLOW:
      testSignals(0);
      break;
    case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_SLOW:
      testSignals(ADS129x_REG_CONFIG2_TEST_AMP);
      break;

    case OPENBCI_CHANNEL_CMD_SET:
      {
        uint8_t channel = cmdBuffer[1] - OPENBCI_CHANNEL_CMD_CHANNEL_1;
        uint8_t powerdown = (cmdBuffer[2] == OPENBCI_CHANNEL_CMD_POWER_OFF) ? ADS129x_REG_CHnSET_PD : 0;
        uint8_t gain = 6;
        switch (cmdBuffer[3]) {
          case OPENBCI_CHANNEL_CMD_GAIN_1: gain = ADS129x_REG_CHnSET_GAIN_1; break;
          case OPENBCI_CHANNEL_CMD_GAIN_2: gain = ADS129x_REG_CHnSET_GAIN_2; break;
          case OPENBCI_CHANNEL_CMD_GAIN_4: gain = ADS129x_REG_CHnSET_GAIN_4; break;
          case OPENBCI_CHANNEL_CMD_GAIN_6: gain = ADS129x_REG_CHnSET_GAIN_6; break;
          case OPENBCI_CHANNEL_CMD_GAIN_8: gain = ADS129x_REG_CHnSET_GAIN_8; break;
          case OPENBCI_CHANNEL_CMD_GAIN_12: gain = ADS129x_REG_CHnSET_GAIN_12; break;
          case OPENBCI_CHANNEL_CMD_GAIN_24: gain = ADS129x_REG_CHnSET_GAIN_12; break;
        }
        uint8_t mux = 0;
        switch (cmdBuffer[4]) {
          case OPENBCI_CHANNEL_CMD_ADC_Normal: mux = ADS129x_REG_CHnSET_MUX_ELECTRODE; break;
          case OPENBCI_CHANNEL_CMD_ADC_Shorted: mux = ADS129x_REG_CHnSET_MUX_SHORTED; break;
          case OPENBCI_CHANNEL_CMD_ADC_BiasMethod: mux = ADS129x_REG_CHnSET_MUX_RLD; break;
          case OPENBCI_CHANNEL_CMD_ADC_MVDD: mux = ADS129x_REG_CHnSET_MUX_MVDD; break;
          case OPENBCI_CHANNEL_CMD_ADC_Temp: mux = ADS129x_REG_CHnSET_MUX_TEMP; break;
          case OPENBCI_CHANNEL_CMD_ADC_TestSig: mux = ADS129x_REG_CHnSET_MUX_TEST; break;
          case OPENBCI_CHANNEL_CMD_ADC_BiasDRP: mux = ADS129x_REG_CHnSET_MUX_RLD_DRP; break;
          case OPENBCI_CHANNEL_CMD_ADC_BiasDRN: mux = ADS129x_REG_CHnSET_MUX_RLD_DRN; break;
        }
        ads->WREG(ADS129x_REG_CH1SET + channel, powerdown | gain | mux);
      }
      break;

    case OPENBCI_SAMPLE_RATE_SET:
      switch (cmdBuffer[1]) {
        case '0':
          ads->WREG(ADS129x_REG_CONFIG1, ads->regData[ADS129x_REG_CONFIG1] & ~ADS129x_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_16KSPS);
          break;
        case '1':
          ads->WREG(ADS129x_REG_CONFIG1, ads->regData[ADS129x_REG_CONFIG1] & ~ADS129x_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_8KSPS);
          break;
        case '2':
          ads->WREG(ADS129x_REG_CONFIG1, ads->regData[ADS129x_REG_CONFIG1] & ~ADS129x_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_4KSPS);
          break;
        case '3':
          ads->WREG(ADS129x_REG_CONFIG1, ads->regData[ADS129x_REG_CONFIG1] & ~ADS129x_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_2KSPS);
          break;
        case '4':
          ads->WREG(ADS129x_REG_CONFIG1, ads->regData[ADS129x_REG_CONFIG1] & ~ADS129x_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_1KSPS);
          break;
        case '5':
          ads->WREG(ADS129x_REG_CONFIG1, ads->regData[ADS129x_REG_CONFIG1] & ~ADS129x_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_500SPS);
          break;
        case '6':
          ads->WREG(ADS129x_REG_CONFIG1, ads->regData[ADS129x_REG_CONFIG1] & ~ADS129x_REG_CONFIG1_RATE_MASK | ADS129x_REG_CONFIG1_500SPS);
          break;
      }
  }
}