/* A driver to read the temperature from the DHT_11 sensor*/
#include <stdint.h>
#include "delay.h"
#include <gpio.h>
#include <platform.h>

//User define
#define DHT_11_Pin PA_0

// A function to wake-up the sensor
void DHT_wakeup(void);

/* A function to take the respond from the sensor
Return 1-> Successful communication
Return 0-> Unsuccessful communication
*/
uint8_t DHT_not_respond(void);
	
/* A function to read a bit from the channel and to recognize it
Return the bit zero or one
*/
uint8_t DHT_ReadBit(void);

/* A function to read the byte and return it
Return the value of each byte
*/
uint8_t ReadByte(void);

/* A function to take the data stream
Store the data on a five byte array
*/
uint8_t DHT_data(void);

/* A function to return an acknowledgement error
Return 1-> Successful data
Return 0-> Unsuccessful data
*/
uint8_t DHT_data_check();

/* A function to read the temperature from the sensor
Returns the temperature (only the integer value)
*/
 uint8_t read_Temp_DHT11();
 
 /* A function to read the decimal valuef rom temperature from the sensor
Returns the decimal temperature (only the integer value)
*/
 
 uint8_t read_decimal_Temp_DHT();

/* A function to read the humidity from the sensor
Returns the humidity (only the integer value)
*/
int read_Hum_DHT11();


