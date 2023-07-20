#include "DHT_11.h"

uint8_t data[5];

// A function to wake-up the sensor
void DHT_wakeup(void){
	gpio_set_mode(DHT_11_Pin, Output);
	gpio_set(DHT_11_Pin, 0);							// Set the channel LOW
	delay_ms(20);															// Delay 18ms
	gpio_set(DHT_11_Pin, 1);							// Set the channel HIGH
	delay_us(30);															// Delay from 40us
}

// A function to take the respond from the sensor
uint8_t DHT_not_respond(void){
	gpio_set_mode(DHT_11_Pin, Input);
	int time = 0;
	
	while (gpio_get(DHT_11_Pin) && time < 100){  //DHT11 at low for 100us 
		time++;
		delay_us(1);
	};	 
	if(time >= 100)										// If the HIGH will remain for more than 100us -> ERROR
		return 1;													// 1-> ERROR
	else 
		time = 0;
  while (!gpio_get(DHT_11_Pin) && time < 100){//Pull up again for 100us 
		time++;
		delay_us(1);
	};
	if(time >= 100){									// If the LOW will remain for more than 100us -> ERROR
		return 1;													//  Acknowledgement ERROR
	}    
	return 0;														// Acknoledgement OK
}

// A function to read a bit from the channel and to recognize it
uint8_t DHT_ReadBit(void){															
	// Wait the channel until it will pull down
	uint8_t time = 0;		// t: microseconds passed 
	while(gpio_get(DHT_11_Pin) && time < 100)//Wait after PullDown
	{
		time++;
		delay_us(1);
	}
	time = 0;
	while(!gpio_get(DHT_11_Pin) && time < 100)//Wait after PullUp
	{
		time++;
		delay_us(1);
	}
	delay_us(40);// delay 40us 
	if(gpio_get(DHT_11_Pin))					// Read the state now, if it is HIGH -> "1"
		return 1;
	else 
	  return 0;		   
}

// A function to read the byte and return it
uint8_t DHT_ReadByte(void){
	uint8_t byte = 0x0;							// The byte that will return
	
	for(int i = 0; i < 8; i++){				// A loop to read 8 bit continuously
		//byte << DHT_ReadBit();		// Read each byte
		byte <<= 1; 
	  byte |= DHT_ReadBit();
	}
	return byte;												// Return the byte
}

// A function to take the data stream
uint8_t DHT_data(void){
	DHT_wakeup();
	if(DHT_not_respond())
		return 0;										// The sensor is not respond
	else{
		for(int i = 0; i < 5; i++){				// Read the 5 byte data stream
			data[i] = DHT_ReadByte();
		}
		return 1;
	}
}

// A function to return an acknowledgement error
uint8_t DHT_data_check(){
	if(data[0] + data[1] + data[2] + data[3] == data[4])
		return 1;
	else
		return 0;
}

// A function to read the temperature from the sensor
uint8_t read_Temp_DHT11(){
	DHT_data();
	if (DHT_data_check())	{					// Check if the sensor measures the temperature correctly
		return (data[2]);
	}
}

// A function to read the decimal value temperature from the sensor
uint8_t read_decimal_Temp_DHT(){
	if (DHT_data_check())	{					// Check if the sensor measures the temperature correctly
		return (data[3]);
	}
}

// A function to read the humidity from the sensor
int read_Hum_DHT11(){
	if (DHT_data_check())						// Check if the sensor measures the humidity correctly
		return (data[0]);
}
