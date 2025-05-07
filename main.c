#include "headers.h"

// ======== CONNECTIONS: ========
/*
		PA6 to SCL
		PA7 to SDA
		
		PF1 to Gear
		PF4 to Buzzer
		
		PB0 to Trig
		PB3 to Echo
		
		PE2 to Potentiometer
		PE3 to Ignition
		
		PD2 to Door
		PE5 to Lock
*/




int main(void)
{
		DIO_Init(PORT_F);
		DIO_Init(PORT_D);

    I2C1_Init(); // (PA6 to SCL and PA7 to SDA)
    //delay_ms(100);
    LCD_init();
    //delay_ms(10);
    LCD_command(LCD_CLEAR);
    //delay_ms(2);
//	int x = 30;
//	    LCD_set_cursor(0, 0);

// LCD_print_int(x);
	
	
	
	Potentiometer_Init(); // PE2 to (Potentiometer)
    delay_ms(100);
	Ultrasonic_Init();
	    delay_ms(100);

	
	DIO_Configure(PORT_E, PIN_THREE, DIGITAL,INPUT, PDR); // IGNITION
	DIO_Configure(PORT_E, PIN_ONE, DIGITAL,INPUT, PDR); // GEAR
	
		DIO_Configure(PORT_D, PIN_TWO, DIGITAL,INPUT, PDR); // DOOR
	DIO_Configure(PORT_E, PIN_FIVE, DIGITAL,INPUT, PDR); // LOCK

	
			DIO_Configure(PORT_F, PIN_ONE, DIGITAL,OUTPUT, PDR); 	//LED
		DIO_Configure(PORT_F, PIN_TWO, DIGITAL,OUTPUT, PDR); 		//LED
		DIO_Configure(PORT_F, PIN_THREE, DIGITAL,OUTPUT, PDR); 	//LED

		DIO_Configure(PORT_F, PIN_FOUR, DIGITAL,OUTPUT, PDR); // buzzer


	
	
//    // Write to first line
//    LCD_set_cursor(0, 0);
//    LCD_write_string("mia mia");

//// Write to second line
//    LCD_set_cursor(1, 0);
//    LCD_write_string("===========");
    
		
		
		xTaskCreate(IgnitionCheckTask,"Ignition Task", 128, NULL, 4, NULL); 
		xTaskCreate(vUltrasonicTask, "UltraTask", 128, NULL, 3, NULL);
		xTaskCreate(GearCheckTask,"Gear Task", 128, NULL, 2, NULL); 
		xTaskCreate(vSpeedTask,"Speed Task", 128, NULL, 2, NULL); 
		xTaskCreate(vBuzzerTask,"BuzzerTask",128,NULL,2,NULL);
		xTaskCreate(DoorCheckTask,"DoorTask",128,NULL,2,NULL);
		xTaskCreate(DisplayTask,"Display Task", 128, NULL, 1, NULL); 
		
		vTaskStartScheduler();
		
    while(1) {
			check_ignition();
			speed = Potentiometer_GetSpeed();
			display();
			delay_ms(100);
			
        
//				float x = Potentiometer_GetSpeed();
//			//LCD_command(LCD_CLEAR);
//	LCD_set_cursor(1, 0);
//	LCD_write_string("Veolcity: ");
//	LCD_print_int(x);
//	if (x < 10){
//			clear_cell(1, 11);
//	}
//	delay_ms(100);
//			
//			
//			display();
			
    }
}
