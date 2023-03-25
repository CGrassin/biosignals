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
  all_defaults();
}
// System commands
void ADS1X9X::SDATAC() {
  digitalWrite(cs_pin, LOW);
  spi->transfer(ADS1x9x_SDATAC);
  digitalWrite(cs_pin, HIGH);
  delayMicroseconds(10);  //must wait at least 4 tCLK cycles after executing this command (Datasheet, pg. 37)
}
void ADS1X9X::RDATAC() {
  digitalWrite(cs_pin, LOW);
  spi->transfer(ADS1x9x_RDATAC);
  digitalWrite(cs_pin, HIGH);
  delayMicroseconds(3);
}
void ADS1X9X::WAKEUP() {
  digitalWrite(cs_pin, LOW);
  spi->transfer(ADS1x9x_WAKEUP);
  digitalWrite(cs_pin, HIGH);
  delayMicroseconds(3);  //must wait 4 tCLK cycles before sending another command (Datasheet, pg. 35)
}
void ADS1X9X::STANDBY() {
  digitalWrite(cs_pin, LOW);
  spi->transfer(ADS1x9x_STANDBY);
  digitalWrite(cs_pin, HIGH);
}
void ADS1X9X::RESET() {  // reset all the registers to default settings
  digitalWrite(cs_pin, LOW);
  spi->transfer(ADS1x9x_RESET);
  delayMicroseconds(12);  //must wait 18 tCLK cycles to execute this command (Datasheet, pg. 35)
  digitalWrite(cs_pin, HIGH);
}
void ADS1X9X::START() {  //start data conversion
  digitalWrite(cs_pin, LOW);
  spi->transfer(ADS1x9x_START);
  digitalWrite(cs_pin, HIGH);
}
void ADS1X9X::STOP() {  //stop data conversion
  digitalWrite(cs_pin, LOW);
  spi->transfer(ADS1x9x_STOP);
  digitalWrite(cs_pin, HIGH);
}
uint8_t ADS1X9X::RREG(byte _address) {      //  reads ONE register at _address
  RREGS(_address, 1);
  return regData[_address];                 // return requested register value
}

// Read more than one register starting at _address
void ADS1X9X::RREGS(uint8_t _address, uint8_t _numRegisters) {
  byte opcode1 = _address + ADS1x9x_RREG;  //  RREG expects 001rrrrr where rrrrr = _address
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

  byte opcode1 = _address + ADS1x9x_WREG;  //  WREG expects 010rrrrr where rrrrr = _address
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
  all_defaults();
}
void ADS1X9X::switch_channel(uint8_t channelnumber, bool powerdown) {
  if (powerdown)
    WREG(ADS1x9x_REG_CH1SET + channelnumber, regData[ADS1x9x_REG_CH1SET + channelnumber] & ~ADS1x9x_REG_CHnSET_PD);
  else
    WREG(ADS1x9x_REG_CH1SET + channelnumber, regData[ADS1x9x_REG_CH1SET + channelnumber] | ADS1x9x_REG_CHnSET_PD);
}
bool ADS1X9X::isContReading() {
  return contReading;
}