#include "headers.h"

// ======== CONNECTIONS: ========
/*
		PA6 to SCL
		PA7 to SDA
		
		PF1 to Gear
		
		PE2 to Potentiometer
		PE3 to Ignition
*/




int main(void)
{
		DIO_Init(PORT_F);
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

	
	DIO_Configure(PORT_E, PIN_THREE, DIGITAL,INPUT, PDR);
	DIO_Configure(PORT_F, PIN_ONE, DIGITAL,INPUT, PDR); // GEAR

	
	
//    // Write to first line
//    LCD_set_cursor(0, 0);
//    LCD_write_string("mia mia");

//// Write to second line
//    LCD_set_cursor(1, 0);
//    LCD_write_string("===========");
    
		
		
		xTaskCreate(IgnitionCheckTask,"Ignition Task", 240, NULL, 3, NULL); 
		xTaskCreate(GearCheckTask,"Gear Task", 240, NULL, 2, NULL); 
		xTaskCreate(vSpeedTask,"Speed Task", 240, NULL, 2, NULL); 
		xTaskCreate(DisplayTask,"Display Task", 240, NULL, 1, NULL); 
		
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
