# Data and command interface

Forked from https://github.com/OpenBCI/Documentation

## Data format

Each packet contains a header followed by a sample counter, followed by 8 ADS channel data, followed by the three axes values of the accelerometer, followed by a footer. The accelerometer data are optional, and don't need to be sent with every packet when used. if unused, the bytes will read 0. This allows for user defined auxiliary data to be sent in the last six bytes before the footer. Also, there may be room for compressing more samples. Here are details on the format.

**Header**

-   Byte 1: 0xA0  
-   Byte 2: Sample Number

**EEG Data**  
Note: values are 24-bit signed, MSB first

-   Bytes 3-5: Data value for EEG channel 1  
-   Bytes 6-8: Data value for EEG channel 2  
-   Bytes 9-11: Data value for EEG channel 3  
-   Bytes 12-14: Data value for EEG channel 4  
-   Bytes 15-17: Data value for EEG channel 5  
-   Bytes 18-20: Data value for EEG channel 6  
-   Bytes 21-23: Data value for EEG channel 6  
-   Bytes 24-26: Data value for EEG channel 8  

**Aux Data**

-   Bytes 27-32: 6 bytes of data defined and parsed based on the **Footer** below

**Footer**  

-   Byte 33: 0xCX where X is 0-F in hex

The following table is sorted by `Stop Byte`. Drivers should use the `Stop Byte` to determine how to parse the 6 `AUX` bytes.

| Stop Byte | Byte 27 | Byte 28 | Byte 29 | Byte 30 | Byte 31 | Byte 32 | Name                                |
| :-------: | :-----: | :-----: | :-----: | :-----: | :-----: | :-----: | ----------------------------------- |
|    0xC0   |   AX1   |   AX0   |   AY1   |   AY0   |   AZ1   |   AZ0   | Standard with accel                 |
|    0xC1   |   UDF   |   UDF   |   UDF   |   UDF   |   UDF   |   UDF   | Standard with raw aux               |
|    0xC2   |   UDF   |   UDF   |   UDF   |   UDF   |   UDF   |   UDF   | User defined                        |
|    0xC3   |    AC   |    AV   |    T3   |    T2   |    T1   |    T0   | Time stamped **_set_** with accel   |
|    0xC4   |    AC   |    AV   |    T3   |    T2   |    T1   |    T0   | Time stamped with accel             |
|    0xC5   |   UDF   |   UDF   |    T3   |    T2   |    T1   |    T0   | Time stamped **_set_** with raw aux |
|    0xC6   |   UDF   |   UDF   |    T3   |    T2   |    T1   |    T0   | Time stamped with raw aux           |

### 24-Bit Signed Data Values

For the EEG data values, you will note that we are transferring the data as a 24-bit signed integer, which is a bit unusual. We are using this number format because it is the native format used by the ADS129x chip that is at the core of the Cyton board. To convert this unusual number format into a more standard 32-bit signed integer, you can steal some ideas from the example Processing (aka, Java) code:

```

int interpret24bitAsInt32(byte[] byteArray) {     
    int newInt = (  
     ((0xFF & byteArray[0]) <! 16) |  
     ((0xFF & byteArray[1]) <! 8) |   
     (0xFF & byteArray[2])  
    );  
    if ((newInt & 0x00800000) > 0) {  
      newInt |= 0xFF000000;  
    } else {  
      newInt &= 0x00FFFFFF;  
    }  
    return newInt;  
}  

```

## Command interface

On startup, the board sends the following text over the radio:

```

OpenBCI V3 8-16 channel
ADS1299 Device ID: 0xXX
LIS3DH Device ID: 0xXX
Firmware: vX.X.X
$$$

```

### Turn Channels OFF

**1 2 3 4 5 6 7 8**  
These ASCII characters turn the respective channels [1-8] off. The channel will read 0.00 when off during streamData mode. These commands work in and out of streamData mode.

**returns** none, there is no confirmation.

### Turn Channels ON

**! @ # $ % ^ &  \***  
These ASCII characters turn the respective channels [1-8] on. The channel will read ADC output values during streamData mode. These commands work in and out of streamData mode.

**returns** none, there is no confirmation.


### Test Signal Control Commands

**0 - = p [ ]**  
Turn **all** available channels on, and connect them to internal test signal. These are useful for self test and calibration. For example, you can measure the internal noise by sending **0** which connects all inputs to an internal GND. If streaming, the stream will be stopped, the proper registers set on the ADS1299, and the stream will be resumed.

-   **0**  Connect to internal GND (VDD - VSS)  
-   **-**  Connect to test signal 1xAmplitude, slow pulse  
-   **=**  Connect to test signal 1xAmplitude, fast pulse  
-   **p**  Connect to DC signal  
-   **\[**  Connect to test signal 2xAmplitude, slow pulse  
-   **]**  Connect to test signal 2xAmplitude, fast pulse  

**Note: Not all possible internal test connections are implemented here.**

**returns** If not streaming, returns `Success: Configured internal test signal.$$$`, if streaming, there is no confirmation.

### Channel Setting Commands

** x (CHANNEL, POWER_DOWN, GAIN_SET, INPUT_TYPE_SET, BIAS_SET, SRB2_SET, SRB1_SET) X **  
Channel Settings commands have six parameters for each ADS channel. To access Channel Settings, first send **x**. The OpenBCI board will then expect the next 7 bytes to be channel settings specific commands. The first byte is the channel number. (If you have the Daisy Module, you can select up to 16 channels to set). The following six ASCII characters are accepted as parameters to set. Lastly, sending **X** will latch the settings to the ADS channel.

**CHANNEL**

-   **1 2 3 4 5 6 7 8**  for single board channel select
-   **Q W E R T Y U I**  for selecting channels on the Daisy Module

**POWER_DOWN**

-   0 = ON (default)
-   1 = OFF    

**GAIN_SET**

-   0 = Gain 1
-   1 = Gain 2
-   2 = Gain 4
-   3 = Gain 6
-   4 = Gain 8
-   5 = Gain 12
-   6 = Gain 24	(default)

**INPUT_TYPE_SET**  
Select the ADC channel input source  

-   0        ADSINPUT_NORMAL     	(default)  
-   1        ADSINPUT_SHORTED          
-   2        ADSINPUT_BIAS_MEAS  
-   3        ADSINPUT_MVDD  
-   4        ADSINPUT_TEMP  
-   5        ADSINPUT_TESTSIG  
-   6        ADSINPUT_BIAS_DRP  
-   7        ADSINPUT_BIAS_DRN  

**BIAS_SET**  
Select to include the channel input in BIAS generation.

-   0 = Remove form BIAS
-   1 = Include in BIAS  (default)  

**SRB2_SET**  
Select to connect this channel's P input to the SRB2 pin. This closes a switch between P input and SRB2 for the given channel, and allows the P input also remain connected to the ADC.  

-   0 = Disconnect this input from SRB2
-   1 = Connect this input to SRB2  (default)  

**SRB1_SET**  
Select to connect all channels' N inputs to SRB1. This effects all pins, and disconnects all N inputs from the ADC.  

-   0 = Disconnect all N inputs from SRB1 (default)
-   1 = Connect all N inputs to SRB1  

**EXAMPLE**

User sends **x  3  0  2  0  0  0  0  X**

'x' enters Channel Settings mode. Channel 3 is set up to be powered up, with gain of 2, normal input, removed from BIAS generation, removed from SRB2, removed from SRB1. The final 'X' latches the settings to the ADS1299 channel settings register.

For firmware `v0` and `v1` it is required that you allow a time delay (&gt;10mS) when setting the channel and parameters. As of `v2.0.0`, you may stack multiple channel settings together such as:

**EXAMPLE**

User sends **x1020000Xx2020000Xx3020000X**

This will set the first three channels.

**returns**

On success:

-   If streaming, no confirmation of success. Note: WiFi Shields will always get a response, even if streaming.
-   If not streaming, returns `Success: Channel set for 3$$$`, where 3 is the channel that was requested to be set.

On failure:

-   If not streaming, NOTE: WiFi shield always sends the following responses without `$$$`
    -   Not enough characters received, `Failure: too few chars$$$` (**example** user sends x102000X)
    -   9th character is not the upper case 'X', `Failure: 9th char not X$$$` (**example** user sends x1020000V)
    -   Too many characters or some other issue, `Failure: Err: too many chars$$$`
-   If not all commands are not received within 1 second, `Timeout processing multi byte message - please send all commands at once as of v2$$$`

### Default Channel Settings

**d** To set all channels to default  

**returns** if not streaming, `updating channel settings to default$$$`

**D** To get a report of the default settings send.

**returns** When you query the default settings, expect to get 6 ASCII characters followed by **$$$**

_Note: Users can change the default channel settings in the initialization function inside the OpenBCI library. Requires re-programming the board._

### LeadOff Impedance Commands

**z (CHANNEL, PCHAN, NCHAN) Z**  
This works similar to the Channel Settings commands. For firmware `v0` and `v1` care must be taken to delay between sending characters, as of `v2.0.0`, you may send as fast as possible in a byte stream. Impedance settings have two parameters for each ADS channel. Impedance is measurable by applying a small 31.5Hz AC signal to the given channel.

-   0 = Test Signal Not Applied (default)
-   1 = Test Signal Applied  

**EXAMPLE**

User sends **z  4  1  0  Z** or `z410Z`

'z' enters Impedance Settings mode. Channel 4 is set up to measure impedance on the P input. The final 'Z' latches the settings to the ADS registers.

**returns**

On success:

-   If streaming, no confirmation of success. Note: WiFi Shields will always get a response, even if streaming.
-   If not streaming, returns `Success: Lead off set for 4$$$`, where 4 is the channel that was requested to be set.

On failure:

-   If not streaming, NOTE: WiFi shield always sends the following responses without `$$$`
    -   Not enough characters received, `Failure: too few chars$$$` (**example** user sends x102000X)
    -   5th character is not the upper case 'X', `Failure: 5th char not Z$$$` (**example** user sends x1020000V)
    -   Too many characters or some other issue, `Failure: Err: too many chars$$$`
-   If not all commands are not received within 1 second, `Timeout processing multi byte message - please send all commands at once as of v2$$$`

<!-- ### SD card Commands

**A S F G H J K L**  
Send to initiate SD card data logging for specified time  

-   A    =      5MIN  
-   S    =      15MIN  
-   F    =      30MIN  
-   G    =      1HR  
-   H    =      2HR  
-   J    =      4HR  
-   K    =      12HR  
-   L    =      24HR  
-   a	   =      about 14 seconds for testing

**j**  
Stop logging data and close SD file   -->

### Stream Data Commands

**b**  
Start streaming data

**returns** none, there is no confirmation. NOTE: when command from WiFi shield, confirmation is `Stream started`.

**s**  
Stop Streaming data  

**returns** none, there is no confirmation. NOTE: when command from WiFi shield, confirmation is `Stream stopped`.

### Miscellaneous

<!-- **?**  
Query register settings  

**returns** Read and report all register settings for the ADS1299 and the LIS3DH. Expect to get a verbose serial output from the OpenBCI Board, followed by **$$$**   -->

**v**

Soft reset for the Board peripherals.

### Sample Rate

**~(COMMAND)**  
This works similar to the Channel Settings commands, however, there is no latching character. Power cycling the OpenBCI board will cause the sample rate to reset back to default of 250Hz.

**COMMAND**

-   0 = 16000 Hz
-   1 = 8000 Hz
-   2 = 4000 Hz
-   3 = 2000 Hz
-   4 = 1000 Hz
-   5 = 500 Hz
-   6 = 250 Hz
-   7 = 125 Hz
-   ~ = Get current sample rate

**EXAMPLE**

First, user sends **\~~**

**returns** `Sample rate is 250Hz$$$`

Then, user sends **~5**

**returns** `Sample rate set to 500Hz$$$`

NOTE: if not all commands are not received within 1 second, `Timeout processing multi byte message - please send all commands at once as of v2$$$`

### Get Version

**V** To get firmware version

**returns** `v3.1.1$$$`

## Unused ASCII Characters

These are currently unused (and user available) characters in the OpenBCI platform:

**9 ( ) \_ o O f g h k l ' " n N M , . (space)**