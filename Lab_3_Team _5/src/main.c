#include <platform.h>
#include <uart.h>
#include <gpio.h>
#include <timer.h>
#include "DHT_11.h"

// User define
#define USER_LED PA_5
#define USER_BUTTON PC_13
#define DHT_Pin PA_0
#define CPU_CLOCK 1000000
#define HIGH_TEMP 25
#define LOW_TEMP 20

// Global variables
uint8_t button_pressed = 0;						// A variable to save how many times the button was pressed
uint8_t AEM[5];															// An array to save the AEM
char temp[3];																	// An array to save the temperature as string to send it via UART
char dec_temp[3];													// An array to save the decimal measurment via UART
char display_rate[1];											// A display rate string to be printed

int i = 0;																				// A helpful variable for the AEM array
uint8_t AEM_sum = 0;										// A variable to store the sum of the two last AEM digits

uint8_t Temp_counter = 0;							// The counter for the temperature measurement
uint8_t Temp_counter_max = 2;			// The maximum value that the counter have to measure

uint8_t Print_counter = 0;								//  The counter to print the temperature on the display
uint8_t Print_counter_max = 2;				//  The maximum value for the print counter

uint8_t temperature = 0;									// A variable to store the temperature that the sensor returns (ASCII value)
uint8_t decimal_temperature = 0;

// Function to initialize the LED state
void LED_init(){
	gpio_set_mode(USER_LED, Output);
	gpio_set(USER_LED, LED_OFF);
}

// ISR for temperatures grater than 25oC
void high_Temp(){
	gpio_set(USER_LED, LED_ON);			// Switch on the LED
}

// ISR for temperatures between 20 and 25oC
void med_Temp(){
	gpio_toggle(USER_LED);
}

//ISR for temperatures lower than 20oC
void low_Temp(){
	gpio_set(USER_LED, LED_OFF);
}

// ISR to process the user's AEM
void AEM_isr(uint8_t rx_data){
	//Save the AEM until the enter character was recognized
	if(rx_data != 10 && (rx_data >=48 && rx_data <=57)){
		AEM[i] = rx_data;										// Store the next rx data
		i++;																				// Go to the next index of the AEM array
	}
	else if(rx_data == 10){
		i--;
		AEM_sum = (AEM[i] - 48) + (AEM[i-1] - 48);				// Add the two last digits of the AEM
		i = 0;																			// Reset the index array
	}
	else{
		uart_print("Wrong AEM!!\n");
		AEM_sum = 2;
	}
}

// ISR if the button was pressed
void Button_isr(){
	button_pressed ++;							 		// If the ISR was called, then means that the button was pressed
	Print_counter = 0;										// Reset the counter value
	
	if(button_pressed == 1){							// At the first time that the button was pressed
		Print_counter_max = AEM_sum;			// Now the timer period will depend from the AEM value
	}
	else{																					// All the other times that the button was pressed
		if(button_pressed % 2 == 0){			// If the button pressed count is even number
			Print_counter_max = 4;
		}
		else{																				// Else if the button pressed count is odd number
			Print_counter_max = 3;
		}
	}
	
	uart_print("\nDisplay rate: ");
	sprintf(display_rate, "%d", Print_counter_max);
	uart_print(display_rate);
	uart_print(" sec\n\n");
}

// ISR that the timer calls
void timer_isr(){
	Temp_counter++;
	Print_counter++;
	
	// Read the temperature from the sensor
	if(Temp_counter == Temp_counter_max){
		temperature = read_Temp_DHT11();													// Read the temperature
		decimal_temperature = read_decimal_Temp_DHT();		// Read the decimal value temperature
		Temp_counter = 0;																										// Reset the timer counter
	}
	
	// Print the temperature value
	if(Print_counter == Print_counter_max){
		uart_print("Temperature: ");
		sprintf(temp, "%d", temperature);
		uart_print(temp);
		uart_print(".");
		sprintf(dec_temp, "%d", decimal_temperature);
		uart_print(dec_temp);
		uart_print(" oC");
		uart_print("\n");
		uart_print("Sensor sampling rate: 2sec\n");
		Print_counter = 0;								// Reset the print counter
	}
	
	// Control the LED based on the temp value
	if(temperature > HIGH_TEMP)
			high_Temp();
		else if(temperature < LOW_TEMP)
			low_Temp();
		else
			med_Temp();
	
}

//Function to initialize the button state
void Button_init(){
	gpio_set_mode(USER_BUTTON, PullUp);
	gpio_set_trigger(USER_BUTTON, Rising);
	gpio_set_callback(USER_BUTTON, Button_isr);
}

int main(){
	// UART Initialization
	uart_init(115200);													// Set the baud rate
	uart_enable();															// Enable the uart communication
	uart_print("Please give me your AEM\n\n");						// A message to user
	
	uart_set_rx_callback(AEM_isr);				// Set the uart callback function
	
	delay_ms(3000);
	
	// Peripherals' init
	LED_init();																	// Initialize the LED
	Button_init();															// Initialize the button
	
	// Timer init
	timer_init(CPU_CLOCK);									// Set up the timer at 1sec
	timer_enable();													// Enable the timer
	timer_set_callback(timer_isr);
	
	__enable_irq();														// Enable the interrupts
	//timer_irq_handler();										// Enable the timer interrupts 
	
	// While (1) loop
	while(1){
		__WFI();																	// Wait for interrupt
	}
}
