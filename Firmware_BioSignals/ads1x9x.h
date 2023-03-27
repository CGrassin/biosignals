#include <sys/_stdint.h>
/*
* This class is the base class for all pin compatible ADS ICs.
* It contains all common registers.
*/
#ifndef __ADS1X9X_INCLUDE
#define __ADS1X9X_INCLUDE

#include <Arduino.h>
#include <SPI.h>

// Opcode Command Definitions
// SYSTEM COMMANDS
#define ADS1X9X_WAKEUP 0x02  /* Wakeup from standby mode */
#define ADS1X9X_STANDBY 0x04 /* Enter standby mode */
#define ADS1X9X_RESET 0x06   /* Reset the device */
#define ADS1X9X_START 0x08   /* Start/restart (synchronize) conversions */
#define ADS1X9X_STOP 0x0A    /* Stop conversion */
// DATA READ COMMANDS
#define ADS1X9X_RDATAC 0x10 /* Enable Read Data Continuous mode. */
#define ADS1X9X_SDATAC 0x11 /* Stop Read Data Continuously mode */
#define ADS1X9X_RDATA 0x12  /* Read data by command; supports multiple read back. */
// REGISTER READ COMMANDS
#define ADS1X9X_RREG 0x20 /* Read n nnnn registers starting at address r rrrr */
#define ADS1X9X_WREG 0x40 /* Write n nnnn registers starting at address r rrrr */
// ---------------------

// Register map (only the common ones)
// DEVICE SETTINGS (READ-ONLY REGISTERS)
#define ADS1X9X_REG_ID 0x00
// GLOBAL SETTINGS ACROSS CHANNELS
#define ADS1X9X_REG_CONFIG1 0x01
#define ADS1X9X_REG_CONFIG2 0x02
#define ADS1X9X_REG_CONFIG3 0x03
#define ADS1X9X_REG_LOFF 0x04
// CHANNEL-SPECIFIC SETTINGS
#define ADS1X9X_REG_CH1SET 0x05
#define ADS1X9X_REG_CH2SET 0x06
#define ADS1X9X_REG_CH3SET 0x07
#define ADS1X9X_REG_CH4SET 0x08
#define ADS1X9X_REG_CH5SET 0x09
#define ADS1X9X_REG_CH6SET 0x0A
#define ADS1X9X_REG_CH7SET 0x0B
#define ADS1X9X_REG_CH8SET 0x0C
#define ADS1X9X_REG_RLD_SENSP 0x0D
#define ADS1X9X_REG_RLD_SENSN 0x0E
#define ADS1X9X_REG_LOFF_SENSP 0x0F
#define ADS1X9X_REG_LOFF_SENSN 0x10
#define ADS1X9X_REG_LOFF_FLIP 0x11
// LEAD-OFF STATUS REGISTERS (READ-ONLY REGISTERS)
#define ADS1X9X_REG_LOFF_STATP 0x12
#define ADS1X9X_REG_LOFF_STATN 0x13
// GPIO AND OTHER REGISTERS
#define ADS1X9X_REG_GPIO 0x14
// 0x15 and 0x16 are device-specific
#define ADS1X9X_REG_CONFIG4 0x17
// 0x18 and 0x19 are device-specific
// ---------------------
// CONFIG BIT MASKS (only the common ones)
#define ADS1X9X_REG_CHnSET_PD 0b10000000 /* Channel power-down */
#define ADS1X9X_REG_CHnSET_MUX_ELECTRODE 0b00000000
#define ADS1X9X_REG_CHnSET_MUX_SHORTED 0b00000001
#define ADS1X9X_REG_CHnSET_MUX_RLD 0b00000010
#define ADS1X9X_REG_CHnSET_MUX_MVDD 0b00000011
#define ADS1X9X_REG_CHnSET_MUX_TEMP 0b00000100
#define ADS1X9X_REG_CHnSET_MUX_TEST 0b00000101
#define ADS1X9X_REG_CHnSET_MUX_RLD_DRP 0b00000110
#define ADS1X9X_REG_CHnSET_MUX_RLD_DRN 0b00000111
#define ADS1X9X_REG_CHnSET_MUX_MASK 0b00000111
#define ADS1X9X_REG_CHnSET_GAIN_MASK 0b01110000
// CONFIG 1
#define ADS1X9X_REG_CONFIG1_MULTI_READBACK 0b01000000 /* Multiple readback mode */
#define ADS1X9X_REG_CONFIG1_CLK_EN 0b00100000         /* Oscillator clock output enabled */
#define ADS1X9X_REG_CONFIG1_RATE_MASK 0b00000111
// CONFIG 2
#define ADS1X9X_REG_CONFIG2_INT_TEST 0b00010000 /* Test signals are generated internally */
#define ADS1X9X_REG_CONFIG2_TEST_AMP 0b00000100
#define ADS1X9X_REG_CONFIG2_TEST_FREQ_FAST 0b00000001
#define ADS1X9X_REG_CONFIG2_TEST_FREQ_DC 0b00000011
// CONFIG 3
#define ADS1X9X_REG_CONFIG3_PD_REFBUF      0b10000000 /* Enable internal reference buffer */
#define ADS1X9X_REG_CONFIG3_BIAS_MEAS      0b00010000 /* BIAS_IN signal is routed to the channel that has the MUX_Setting 010 (VREF) */
#define ADS1X9X_REG_CONFIG3_BIASREF_INT    0b00001000 /* BIASREF signal (AVDD + AVSS) / 2 generated internally */
#define ADS1X9X_REG_CONFIG3_PD_BIAS        0b00000100 /* BIAS buffer power powered */
#define ADS1X9X_REG_CONFIG3_BIAS_LOFF_SENS 0b00000010 /* BIAS sense function enabled. */
#define ADS1X9X_REG_CONFIG3_BIAS_STAT      0b00000001 /* BIAS lead-off status connected. */
// LOFF
#define ADS1X9X_REG_LOFF_COMP_TH_95 0b00000000
#define ADS1X9X_REG_LOFF_COMP_TH_92_5 0b00100000
#define ADS1X9X_REG_LOFF_COMP_TH_90 0b01000000
#define ADS1X9X_REG_LOFF_COMP_TH_87_5 0b01100000
#define ADS1X9X_REG_LOFF_COMP_TH_85 0b10000000
#define ADS1X9X_REG_LOFF_COMP_TH_80 0b10100000
#define ADS1X9X_REG_LOFF_COMP_TH_75 0b11000000
#define ADS1X9X_REG_LOFF_COMP_TH_70 0b11100000
#define ADS1X9X_REG_ILEAD_OFF_6_nA  0b00000000
#define ADS1X9X_REG_ILEAD_OFF_24_nA 0b00000100
#define ADS1X9X_REG_ILEAD_OFF_6_uA  0b00001000
#define ADS1X9X_REG_ILEAD_OFF_24_uA 0b00001100
#define ADS1X9X_REG_FLEAD_OFF_DC 0b00000000
#define ADS1X9X_REG_FLEAD_OFF_AC_7_8HZ 0b00000001
#define ADS1X9X_REG_FLEAD_OFF_AC_31_2HZ 0b00000010
#define ADS1X9X_REG_FLEAD_OFF_AC_FdrDIV4 0b00000011
// ---------------------

/**
Abstract interface to support all ADS ICs.
*/
class ADS1X9X {
public:
  uint8_t regData[24];  // array is used to mirror register data
  uint8_t status[3]; // contains sampled last data
  uint8_t data[3 * 8]; // contains sampled last data

  enum INPUT_TYPE{
    INPUT_NORMAL,
    INPUT_SHORTED,
    INPUT_RLD,
    INPUT_MVDD,
    INPUT_TEMP,
    INPUT_TEST,
    INPUT_RLD_DRP,
    INPUT_RLD_DRN
  };

  enum SAMPLE_RATE {
    SAMPLE_RATE_16000,
    SAMPLE_RATE_8000,
    SAMPLE_RATE_4000,
    SAMPLE_RATE_2000,
    SAMPLE_RATE_1000,
    SAMPLE_RATE_500,
    SAMPLE_RATE_250,
    SAMPLE_RATE_125
  };

  ADS1X9X(int cs_pin_set, int drdy_pin_set, int reset_pin_set, SPIClass* spi_set);
  void init();

  // System commands)
  void SDATAC();
  void RDATAC();
  void WAKEUP();
  void STANDBY();
  void RESET();
  void START();
  void STOP();
  uint8_t RREG(uint8_t _address);
  void RREGS(uint8_t _address, uint8_t _numRegisters);
  void WREG(uint8_t _address,  uint8_t _value);
  void WREGS(uint8_t _address, uint8_t _numRegisters);
  
  // Interface
  void start_stream();
  void stop_stream();
  void hard_reset();
  void soft_reset();
  void switch_channel(uint8_t channelnumber, bool powerdown);
  bool isContReading();
  virtual void read_data();
  virtual const char * getRegisterName(uint8_t _address);
  virtual uint8_t set_channel_settings(uint8_t channelnumber, bool powerdown, uint8_t gain, INPUT_TYPE mux, bool bias, bool srb2, bool srb1);

  // Abstract functions (IC-specific)
  virtual uint8_t set_sample_rate(SAMPLE_RATE sr) = 0; // Return the downsampling rate required to reach the desired SPS

protected:
  int cs_pin;
  int drdy_pin;
  int reset_pin;
  SPIClass* spi;
  bool contReading = false;

  // Abstract function (IC-specific)
  virtual void all_defaults() = 0;
};

#endif