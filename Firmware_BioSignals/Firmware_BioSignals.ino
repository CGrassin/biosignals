#include <SPI.h>
#include <SdFat.h>
#include "openbci.h"

// Type of ADC:
// 0 for ADS1194, ADS1196, ADS1198 (UNTESTED).
// 1 for ADS1294, ADS1294R, ADS1296, ADS1296R, ADS1298, ADS1298R.
// 2 for ADS1299, ADS1299-4, ADS1299-6 (UNTESTED).
#define ADS_CHIP_TYPE 1

// Pinout for U2 (STM32F103C8Tx)
#define PIN_FE_DRDY PA1
#define PIN_FE_CS PA4
#define PIN_FE_RESET PB0
#define PIN_SPI1_SCLK PA5
#define PIN_SPI1_MISO PA6
#define PIN_SPI1_MOSI PA7

#define PIN_RX_ISO PA2 /* Connected to CH340 RX (TX on STM32) */
#define PIN_TX_ISO PA3 /* Connected to CH340 TX (RX on STM32) */

#define PIN_SD_CS PA15
#define PIN_SPI2_SCLK PB13
#define PIN_SPI2_MISO PB14
#define PIN_SPI2_MOSI PB15

#define PIN_BT_KEY PB3 /* Unused */
#define PIN_BT_RX PB6 /* Connected to HC-06 RX (TX on STM32) */
#define PIN_BT_TX PB7 /* Connected to HC-06 TX (RX on STM32) */

#define PIN_I2C_SCL PB8 /* Unused */
#define PIN_I2C_SDA PB9 /* Unused */

// TODO
// SD CARD

// CONFIG
//#define SD_CARD_ENABLE
#define SD_CARD_FILENAME "data.csv" // Warning: 8.3 format mandatory TBC
SPIClass SD_SPI2(PIN_SPI2_MOSI,PIN_SPI2_MISO,PIN_SPI2_SCLK);
File sd_file;
#define SD2_CONFIG SdSpiConfig(PIN_SD_CS, DEDICATED_SPI, SD_SCK_MHZ(18), &SD_SPI2)
SdFat SD;

HardwareSerial USB_SERIAL(PIN_TX_ISO, PIN_RX_ISO);
#define USB_SERIAL_SPEED 115200

HardwareSerial BT_SERIAL(PIN_BT_TX, PIN_BT_RX);
#define BT_SERIAL_SPEED 9600

SPIClass SPI1_FE(PIN_SPI1_MOSI,PIN_SPI1_MISO,PIN_SPI1_SCLK);
#if ADS_CHIP_TYPE == 0
  #include "ads119x.h"
  ADS119x ads = ADS119x(PIN_FE_CS, PIN_FE_DRDY, PIN_FE_RESET, &SPI1_FE);
#elif ADS_CHIP_TYPE == 1
  #include "ads129x.h"
  ADS129X ads = ADS129X(PIN_FE_CS, PIN_FE_DRDY, PIN_FE_RESET, &SPI1_FE);
#else
  #include "ads1299.h"
  ADS1299 ads = ADS1299(PIN_FE_CS, PIN_FE_DRDY, PIN_FE_RESET, &SPI1_FE);
#endif
OpenBCI openbci = OpenBCI(&ads, &USB_SERIAL);

void setup() {
  // USB ISO
  USB_SERIAL.begin(USB_SERIAL_SPEED);

  // Bluetooth
  BT_SERIAL.begin(BT_SERIAL_SPEED);

  // SD card (open card and file)
  #ifdef SD_CARD_ENABLE 
    if (SD.begin(SD2_CONFIG)) {
      sd_file = SD.open(SD_CARD_FILENAME, FILE_WRITE);
      if(sd_file){
        sd_file.println("Success!");
        sd_file.close();
        USB_SERIAL.println("Wrote to file");
      }
      else
       USB_SERIAL.println("Failed to create file");
    }
    else
       USB_SERIAL.println("Failed open SD");
  #endif

  // Init the OpenBCI and ADS
  openbci.init();
}

void loop() {
  if(ads.read_data()){
    openbci.sendData(ads.data);
  }
  openbci.readCMD();
}
