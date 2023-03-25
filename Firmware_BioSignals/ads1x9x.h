/*
* This class is the base class for all pin compatible ADS ICs.
* It contains all common registers.
*/
#ifndef __ADS1x9X_INCLUDE
#define __ADS1x9X_INCLUDE

#include <Arduino.h>
#include <SPI.h>

// Opcode Command Definitions
// SYSTEM COMMANDS
#define ADS1x9x_WAKEUP 0x02  /* Wakeup from standby mode */
#define ADS1x9x_STANDBY 0x04 /* Enter standby mode */
#define ADS1x9x_RESET 0x06   /* Reset the device */
#define ADS1x9x_START 0x08   /* Start/restart (synchronize) conversions */
#define ADS1x9x_STOP 0x0A    /* Stop conversion */
// DATA READ COMMANDS
#define ADS1x9x_RDATAC 0x10 /* Enable Read Data Continuous mode. */
#define ADS1x9x_SDATAC 0x11 /* Stop Read Data Continuously mode */
#define ADS1x9x_RDATA 0x12  /* Read data by command; supports multiple read back. */
// REGISTER READ COMMANDS
#define ADS1x9x_RREG 0x20 /* Read n nnnn registers starting at address r rrrr */
#define ADS1x9x_WREG 0x40 /* Write n nnnn registers starting at address r rrrr */
// ---------------------

// Register map (only the common ones)
// DEVICE SETTINGS (READ-ONLY REGISTERS)
#define ADS1x9x_REG_ID 0x00
// GLOBAL SETTINGS ACROSS CHANNELS
#define ADS1x9x_REG_CONFIG1 0x01
#define ADS1x9x_REG_CONFIG2 0x02
#define ADS1x9x_REG_CONFIG3 0x03
#define ADS1x9x_REG_LOFF 0x04
// CHANNEL-SPECIFIC SETTINGS
#define ADS1x9x_REG_CH1SET 0x05
#define ADS1x9x_REG_CH2SET 0x06
#define ADS1x9x_REG_CH3SET 0x07
#define ADS1x9x_REG_CH4SET 0x08
#define ADS1x9x_REG_CH5SET 0x09
#define ADS1x9x_REG_CH6SET 0x0A
#define ADS1x9x_REG_CH7SET 0x0B
#define ADS1x9x_REG_CH8SET 0x0C
#define ADS1x9x_REG_RLD_SENSP 0x0D
#define ADS1x9x_REG_RLD_SENSN 0x0E
#define ADS1x9x_REG_LOFF_SENSP 0x0F
#define ADS1x9x_REG_LOFF_SENSN 0x10
#define ADS1x9x_REG_LOFF_FLIP 0x11
// LEAD-OFF STATUS REGISTERS (READ-ONLY REGISTERS)
#define ADS1x9x_REG_LOFF_STATP 0x12
#define ADS1x9x_REG_LOFF_STATN 0x13
// GPIO AND OTHER REGISTERS
#define ADS1x9x_REG_GPIO 0x14
#define ADS1x9x_REG_CONFIG4 0x17
// ---------------------

// CONFIG BIT MASKS
#define ADS1x9x_REG_CHnSET_PD 0b10000000 /* Channel power-down */
#define ADS1x9x_REG_CHnSET_MUX_ELECTRODE 0b00000000
#define ADS1x9x_REG_CHnSET_MUX_SHORTED 0b00000001
#define ADS1x9x_REG_CHnSET_MUX_RLD 0b00000010
#define ADS1x9x_REG_CHnSET_MUX_MVDD 0b00000011
#define ADS1x9x_REG_CHnSET_MUX_TEMP 0b00000100
#define ADS1x9x_REG_CHnSET_MUX_TEST 0b00000101
#define ADS1x9x_REG_CHnSET_MUX_RLD_DRP 0b00000110
#define ADS1x9x_REG_CHnSET_MUX_RLD_DRN 0b00000111
#define ADS1x9x_REG_CHnSET_MUX_MASK 0b00000111
#define ADS1x9x_REG_CHnSET_GAIN_MASK 0b01110000


/**
Abstract interface to support all ADS ICs.
*/
class ADS1X9X {
protected:
  int cs_pin;
  int drdy_pin;
  int reset_pin;
  SPIClass* spi;
  bool contReading = false;

public:
  uint8_t regData[24];  // array is used to mirror register data
  uint8_t status[3]; // contains sampled last data
  uint8_t data[3 * 8]; // contains sampled last data

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
  
  // Abstract functions (IC-specific)
  virtual void all_defaults() = 0;
  virtual void channel_defaults() = 0;
  //virtual void set_sample_rate() = 0; // TODO
  //virtual void set_channel_settings() = 0; // TODO
};

#endif