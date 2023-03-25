/*
* This class is the OpenBCI interface definition for the board (commands and data output interface).
* Adapted from https://github.com/OpenBCI/OpenBCI_Cyton_Library/blob/master/OpenBCI_32bit_Library_Definitions.h
* Interface documented: https://docs.openbci.com/Cyton/CytonSDK/ and https://docs.openbci.com/Cyton/CytonDataFormat/
*/
#ifndef __OpenBCI_INCLUDE
#define __OpenBCI_INCLUDE

#include "ads1x9x.h"
#include "ads129x.h" // TODO remove this dependancy, ADS1X9X shall be sufficient
#include <Arduino.h>

#define PCKT_START 0xA0  // prefix for data packet error checking
#define MULTI_CHAR_COMMAND_TIMEOUT_MS 1000

// OPENBCI_COMMANDS
/** Turning channels off */
#define OPENBCI_CHANNEL_OFF_1 '1'
#define OPENBCI_CHANNEL_OFF_2 '2'
#define OPENBCI_CHANNEL_OFF_3 '3'
#define OPENBCI_CHANNEL_OFF_4 '4'
#define OPENBCI_CHANNEL_OFF_5 '5'
#define OPENBCI_CHANNEL_OFF_6 '6'
#define OPENBCI_CHANNEL_OFF_7 '7'
#define OPENBCI_CHANNEL_OFF_8 '8'
#define OPENBCI_CHANNEL_OFF_9 'q'
#define OPENBCI_CHANNEL_OFF_10 'w'
#define OPENBCI_CHANNEL_OFF_11 'e'
#define OPENBCI_CHANNEL_OFF_12 'r'
#define OPENBCI_CHANNEL_OFF_13 't'
#define OPENBCI_CHANNEL_OFF_14 'y'
#define OPENBCI_CHANNEL_OFF_15 'u'
#define OPENBCI_CHANNEL_OFF_16 'i'

/** Turn channels on */
#define OPENBCI_CHANNEL_ON_1 '!'
#define OPENBCI_CHANNEL_ON_2 '@'
#define OPENBCI_CHANNEL_ON_3 '#'
#define OPENBCI_CHANNEL_ON_4 '$'
#define OPENBCI_CHANNEL_ON_5 '%'
#define OPENBCI_CHANNEL_ON_6 '^'
#define OPENBCI_CHANNEL_ON_7 '&'
#define OPENBCI_CHANNEL_ON_8 '*'
#define OPENBCI_CHANNEL_ON_9 'Q'
#define OPENBCI_CHANNEL_ON_10 'W'
#define OPENBCI_CHANNEL_ON_11 'E'
#define OPENBCI_CHANNEL_ON_12 'R'
#define OPENBCI_CHANNEL_ON_13 'T'
#define OPENBCI_CHANNEL_ON_14 'Y'
#define OPENBCI_CHANNEL_ON_15 'U'
#define OPENBCI_CHANNEL_ON_16 'I'

/** Test Signal Control Commands
 * 1x - Voltage will be 1 * (VREFP - VREFN) / 2.4 mV
 * 2x - Voltage will be 2 * (VREFP - VREFN) / 2.4 mV
 */
#define OPENBCI_TEST_SIGNAL_CONNECT_TO_DC 'p'
#define OPENBCI_TEST_SIGNAL_CONNECT_TO_GROUND '0'
#define OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_FAST '='
#define OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_SLOW '-'
#define OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_FAST ']'
#define OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_SLOW '['

/** Channel Setting Commands */
#define OPENBCI_CHANNEL_CMD_ADC_Normal '0'
#define OPENBCI_CHANNEL_CMD_ADC_Shorted '1'
#define OPENBCI_CHANNEL_CMD_ADC_BiasDRP '6'
#define OPENBCI_CHANNEL_CMD_ADC_BiasDRN '7'
#define OPENBCI_CHANNEL_CMD_ADC_BiasMethod '2'
#define OPENBCI_CHANNEL_CMD_ADC_MVDD '3'
#define OPENBCI_CHANNEL_CMD_ADC_Temp '4'
#define OPENBCI_CHANNEL_CMD_ADC_TestSig '5'
#define OPENBCI_CHANNEL_CMD_BIAS_INCLUDE '1'
#define OPENBCI_CHANNEL_CMD_BIAS_REMOVE '0'
#define OPENBCI_CHANNEL_CMD_CHANNEL_1 '1'
#define OPENBCI_CHANNEL_CMD_CHANNEL_2 '2'
#define OPENBCI_CHANNEL_CMD_CHANNEL_3 '3'
#define OPENBCI_CHANNEL_CMD_CHANNEL_4 '4'
#define OPENBCI_CHANNEL_CMD_CHANNEL_5 '5'
#define OPENBCI_CHANNEL_CMD_CHANNEL_6 '6'
#define OPENBCI_CHANNEL_CMD_CHANNEL_7 '7'
#define OPENBCI_CHANNEL_CMD_CHANNEL_8 '8'
#define OPENBCI_CHANNEL_CMD_CHANNEL_9 'Q'
#define OPENBCI_CHANNEL_CMD_CHANNEL_10 'W'
#define OPENBCI_CHANNEL_CMD_CHANNEL_11 'E'
#define OPENBCI_CHANNEL_CMD_CHANNEL_12 'R'
#define OPENBCI_CHANNEL_CMD_CHANNEL_13 'T'
#define OPENBCI_CHANNEL_CMD_CHANNEL_14 'Y'
#define OPENBCI_CHANNEL_CMD_CHANNEL_15 'U'
#define OPENBCI_CHANNEL_CMD_CHANNEL_16 'I'
#define OPENBCI_CHANNEL_CMD_GAIN_1 '0'
#define OPENBCI_CHANNEL_CMD_GAIN_2 '1'
#define OPENBCI_CHANNEL_CMD_GAIN_4 '2'
#define OPENBCI_CHANNEL_CMD_GAIN_6 '3'
#define OPENBCI_CHANNEL_CMD_GAIN_8 '4'
#define OPENBCI_CHANNEL_CMD_GAIN_12 '5'
#define OPENBCI_CHANNEL_CMD_GAIN_24 '6'
#define OPENBCI_CHANNEL_CMD_LATCH 'X'
#define OPENBCI_CHANNEL_CMD_POWER_OFF '1'
#define OPENBCI_CHANNEL_CMD_POWER_ON '0'
#define OPENBCI_CHANNEL_CMD_SET 'x'
#define OPENBCI_CHANNEL_CMD_SRB1_CONNECT '1'
#define OPENBCI_CHANNEL_CMD_SRB1_DISCONNECT '0'
#define OPENBCI_CHANNEL_CMD_SRB2_CONNECT '1'
#define OPENBCI_CHANNEL_CMD_SRB2_DISCONNECT '0'

/** Default Channel Settings */
#define OPENBCI_CHANNEL_DEFAULT_ALL_SET 'd'
#define OPENBCI_CHANNEL_DEFAULT_ALL_REPORT 'D'

/** LeadOff Impedance Commands */
#define OPENBCI_CHANNEL_IMPEDANCE_LATCH 'Z'
#define OPENBCI_CHANNEL_IMPEDANCE_SET 'z'
#define OPENBCI_CHANNEL_IMPEDANCE_TEST_SIGNAL_APPLIED '1'
#define OPENBCI_CHANNEL_IMPEDANCE_TEST_SIGNAL_APPLIED_NOT '0'

/** SD card Commands */
#define OPENBCI_SD_LOG_FOR_HOUR_1 'G'
#define OPENBCI_SD_LOG_FOR_HOUR_2 'H'
#define OPENBCI_SD_LOG_FOR_HOUR_4 'J'
#define OPENBCI_SD_LOG_FOR_HOUR_12 'K'
#define OPENBCI_SD_LOG_FOR_HOUR_24 'L'
#define OPENBCI_SD_LOG_FOR_MIN_5 'A'
#define OPENBCI_SD_LOG_FOR_MIN_15 'S'
#define OPENBCI_SD_LOG_FOR_MIN_30 'F'
#define OPENBCI_SD_LOG_FOR_SEC_14 'a'
#define OPENBCI_SD_LOG_STOP 'j'

/** Stream Data Commands */
#define OPENBCI_STREAM_START 'b'
#define OPENBCI_STREAM_STOP 's'

/** Miscellaneous */
#define OPENBCI_MISC_QUERY_REGISTER_SETTINGS '?'
#define OPENBCI_MISC_SOFT_RESET 'v'

/** 16 Channel Commands */
#define OPENBCI_CHANNEL_MAX_NUMBER_8 'c'
#define OPENBCI_CHANNEL_MAX_NUMBER_16 'C'

#define OPENBCI_BOARD_MODE_SET '/'

#define OPENBCI_GET_VERSION 'V'

/** Set sample rate */
#define OPENBCI_SAMPLE_RATE_SET '~'

/** Insert marker into the stream */
#define OPENBCI_INSERT_MARKER '`'

/** Sync Clocks */
#define OPENBCI_TIME_SET '<'
#define OPENBCI_TIME_STOP '>'

/** Wifi Stuff */
#define OPENBCI_WIFI_ATTACH '{'
#define OPENBCI_WIFI_REMOVE '}'
#define OPENBCI_WIFI_STATUS ':'
#define OPENBCI_WIFI_RESET ';'

// Custom
#define OPENBCI_BUFFER_LENGTH 10
#define OPENBCI_PACKAGE_STOP_BYTE 0xC2

// Messages
#define OPENBCI_FIRMWARE_VERSION "v3.0.0"
#define OPENBCI_CMD_TIMEOUT_MSG "Timeout processing multi byte message - please send all commands at once as of v2$$$"
#define OPENBCI_CMD_SET_SAMPLERATE_MSG_0 "Sample rate set to "
#define OPENBCI_CMD_SET_SAMPLERATE_MSG_1 "Hz$$$"
#define OPENBCI_CMD_TESTSIGNAL_SUCCESS_MSG "Success: Configured internal test signal.$$$"
#define OPENBCI_CMD_CHANNEL_DEFAULTS_UPDATE_MSG "updating channel settings to default$$$"
#define OPENBCI_CMD_CHANNEL_DEFAULTS_MSG "050000$$$"

class OpenBCI {
private:
  ADS1X9X* ads;
  HardwareSerial* serial;
  unsigned long multiCharCmdTimeout = 0;  // the timeout in millis of the current multi char command
  char cmdBuffer[OPENBCI_BUFFER_LENGTH];
  byte cmdIdx = 0; // Current index in the command buffer
  uint8_t package_counter = 0; // Package counter
  uint8_t sample_counter = 0; // Sample counter
  uint8_t downsampling_factor; // Divides the data rate by this factor (e.g. 500 SPS and setting this to 2 = 250 messages per second)

public:
  OpenBCI(ADS1X9X* ads, HardwareSerial* serial);
  void startUpMessage();
  void sendData(uint8_t* value);
  void readCMD();

private:
  void testSignals(uint8_t config2);
  static bool isSingleCharCmd(char cmd);
  void processCMD();
  void printHex(byte _data);
};
#endif