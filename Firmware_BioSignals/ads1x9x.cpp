#include "ads1x9x.h"

ADS1X9X::ADS1X9X(int cs_pin_set, int drdy_pin_set, int reset_pin_set, SPIClass* spi_set) {
  this->cs_pin = cs_pin_set;
  this->reset_pin = reset_pin_set;
  this->drdy_pin = drdy_pin_set;
  this->spi = spi_set;
}
void ADS1X9X::init() {
  // Pins
  pinMode(drdy_pin, INPUT);
  pinMode(cs_pin, OUTPUT);
  pinMode(reset_pin, OUTPUT);
  digitalWrite(cs_pin, HIGH);
  // SPI
  spi->begin();
  spi->setBitOrder(MSBFIRST);
  spi->setClockDivider(SPI_CLOCK_DIV4);
  spi->setDataMode(SPI_MODE1);
  // RESET THE CHIP
  hard_reset();
  stop_stream();
  
  RREGS(0, 24);  // Fetch registers
  all_defaults();
}
// System commands
void ADS1X9X::SDATAC() {
  digitalWrite(cs_pin, LOW);
  spi->transfer(ADS1X9X_SDATAC);
  digitalWrite(cs_pin, HIGH);
  delayMicroseconds(10);  //must wait at least 4 tCLK cycles after executing this command (Datasheet, pg. 37)
}
void ADS1X9X::RDATAC() {
  digitalWrite(cs_pin, LOW);
  spi->transfer(ADS1X9X_RDATAC);
  digitalWrite(cs_pin, HIGH);
  delayMicroseconds(3);
}
void ADS1X9X::WAKEUP() {
  digitalWrite(cs_pin, LOW);
  spi->transfer(ADS1X9X_WAKEUP);
  digitalWrite(cs_pin, HIGH);
  delayMicroseconds(3);  //must wait 4 tCLK cycles before sending another command (Datasheet, pg. 35)
}
void ADS1X9X::STANDBY() {
  digitalWrite(cs_pin, LOW);
  spi->transfer(ADS1X9X_STANDBY);
  digitalWrite(cs_pin, HIGH);
}
void ADS1X9X::RESET() {  // reset all the registers to default settings
  digitalWrite(cs_pin, LOW);
  spi->transfer(ADS1X9X_RESET);
  delayMicroseconds(12);  //must wait 18 tCLK cycles to execute this command (Datasheet, pg. 35)
  digitalWrite(cs_pin, HIGH);
}
void ADS1X9X::START() {  //start data conversion
  digitalWrite(cs_pin, LOW);
  spi->transfer(ADS1X9X_START);
  digitalWrite(cs_pin, HIGH);
}
void ADS1X9X::STOP() {  //stop data conversion
  digitalWrite(cs_pin, LOW);
  spi->transfer(ADS1X9X_STOP);
  digitalWrite(cs_pin, HIGH);
}
uint8_t ADS1X9X::RREG(byte _address) {      //  reads ONE register at _address
  RREGS(_address, 1);
  return regData[_address];                 // return requested register value
}

// Read more than one register starting at _address
void ADS1X9X::RREGS(uint8_t _address, uint8_t _numRegisters) {
  byte opcode1 = _address + ADS1X9X_RREG;  //  RREG expects 001rrrrr where rrrrr = _address
  digitalWrite(cs_pin, LOW);               //  open SPI
  spi->transfer(opcode1);                  //  opcode1
  spi->transfer(_numRegisters - 1);    //  opcode2
  for (int i = 0; i < _numRegisters; i++) {
    regData[_address + i] = spi->transfer(0x00);  //  add register byte to mirror array
  }
  digitalWrite(cs_pin, HIGH);  //  close SPI
}

void ADS1X9X::WREG(uint8_t _address, uint8_t _value) {  //  Write ONE register at _address
  regData[_address] = _value;                     //  update the mirror array
  WREGS(_address, 1);
}

void ADS1X9X::WREGS(uint8_t _address, uint8_t _numRegisters) {
  bool wasReading = contReading;
  if (contReading)
    stop_stream();

  byte opcode1 = _address + ADS1X9X_WREG;  //  WREG expects 010rrrrr where rrrrr = _address
  digitalWrite(cs_pin, LOW);               //  open SPI
  spi->transfer(opcode1);                  //  Send WREG command & address
  spi->transfer(_numRegisters-1);    //  Send number of registers to read -1
  for (int i = 0; i < _numRegisters; i++) {
    spi->transfer(regData[_address + i]);  //  Write to the registers
  }
  digitalWrite(cs_pin, HIGH);

  if (wasReading)
    start_stream();
}
void ADS1X9X::start_stream() {
  START();
  RDATAC();
  contReading = true;
}
void ADS1X9X::stop_stream() {
  STOP();
  SDATAC();
  contReading = false;
}
void ADS1X9X::hard_reset(){
  digitalWrite(reset_pin, LOW);  // reset pin connected to both ADS ICs
  delayMicroseconds(4);        // toggle reset pin
  digitalWrite(reset_pin, HIGH); // this will reset the Daisy if it is present
  delayMicroseconds(20);       // recommended to wait 18 Tclk before using device (~8uS);
  all_defaults();
}
void ADS1X9X::soft_reset(){
  RESET();
  delayMicroseconds(1000); // give some time for the IC to reset
  all_defaults();
}
void ADS1X9X::switch_channel(uint8_t channelnumber, bool powerdown) {
  if (powerdown)
    WREG(ADS1X9X_REG_CH1SET + channelnumber, regData[ADS1X9X_REG_CH1SET + channelnumber] & ~ADS1X9X_REG_CHnSET_PD);
  else
    WREG(ADS1X9X_REG_CH1SET + channelnumber, regData[ADS1X9X_REG_CH1SET + channelnumber] | ADS1X9X_REG_CHnSET_PD);
}
bool ADS1X9X::isContReading() {
  return contReading;
}
bool ADS1X9X::read_data() {
  if(!digitalRead(drdy_pin)){
    digitalWrite(cs_pin, LOW);
    for (int i = 0; i < 3; i++) 
      this->status[i] = spi->transfer(0);
    for (int i = 0; i < 8 * 3; i++)
      this->data[i] = spi->transfer(0);
    digitalWrite(cs_pin, HIGH);
    return true;
  }
  return false;
}
const char * ADS1X9X::getRegisterName(uint8_t _address){
  switch (_address) {
    case ADS1X9X_REG_ID: return "ADS_ID";
    case ADS1X9X_REG_CONFIG1: return "CONFIG1";
    case ADS1X9X_REG_CONFIG2: return "CONFIG2";
    case ADS1X9X_REG_CONFIG3: return "CONFIG3";
    case ADS1X9X_REG_LOFF: return "LOFF";
    case ADS1X9X_REG_CH1SET: return "CH1SET";
    case ADS1X9X_REG_CH2SET: return "CH2SET";
    case ADS1X9X_REG_CH3SET: return "CH3SET";
    case ADS1X9X_REG_CH4SET: return "CH4SET";
    case ADS1X9X_REG_CH5SET: return "CH5SET";
    case ADS1X9X_REG_CH6SET: return "CH6SET";
    case ADS1X9X_REG_CH7SET: return "CH7SET";
    case ADS1X9X_REG_CH8SET: return "CH8SET";
    case ADS1X9X_REG_RLD_SENSP: return "BIAS_SENSP";
    case ADS1X9X_REG_RLD_SENSN: return "BIAS_SENSN";
    case ADS1X9X_REG_LOFF_SENSP: return "LOFF_SENSP";
    case ADS1X9X_REG_LOFF_SENSN: return "LOFF_SENSN";
    case ADS1X9X_REG_LOFF_FLIP: return "LOFF_FLIP";
    case ADS1X9X_REG_LOFF_STATP: return "LOFF_STATP";
    case ADS1X9X_REG_LOFF_STATN: return "LOFF_STATN";
    case ADS1X9X_REG_GPIO: return "GPIO";
    case ADS1X9X_REG_CONFIG4: return "CONFIG4";
    default: return "RESERVED";
  }
}
uint8_t ADS1X9X::set_channel_settings(uint8_t channelnumber, bool powerdown, uint8_t gain, INPUT_TYPE mux, bool bias, bool srb2, bool srb1) {
  uint8_t registerValue = 0;

  if (powerdown)
    registerValue |= ADS1X9X_REG_CHnSET_PD;

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
    WREG(ADS1X9X_REG_RLD_SENSP, this->regData[ADS1X9X_REG_RLD_SENSP] | (0x01 << channelnumber));
    WREG(ADS1X9X_REG_RLD_SENSN, this->regData[ADS1X9X_REG_RLD_SENSN] | (0x01 << channelnumber));
  } else {
    WREG(ADS1X9X_REG_RLD_SENSP, this->regData[ADS1X9X_REG_RLD_SENSP] & ~(0x01 << channelnumber));
    WREG(ADS1X9X_REG_RLD_SENSN, this->regData[ADS1X9X_REG_RLD_SENSN] & ~(0x01 << channelnumber));
  }

  return registerValue;
}