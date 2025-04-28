#include "headers.h"

// ======== CONNECTIONS: ========
/*
		PA6 to SCL
		PA7 to SDA
		
		PE2 to Potentiometer
*/

int main(void)
{
    I2C1_Init(); // (PA6 to SCL and PA7 to SDA)
    //delay_ms(100);
    LCD_init();
    //delay_ms(10);
    LCD_command(LCD_CLEAR);
    //delay_ms(2);
//	int x = 30;
//	    LCD_set_cursor(0, 0);

// LCD_print_int(x);
	
	
	
	Potentiometer_Init();
    delay_ms(100);

	
	
	
	
    // Write to first line
    LCD_set_cursor(0, 0);
    LCD_write_string("mia mia");

// Write to second line
    LCD_set_cursor(1, 0);
    LCD_write_string("===========");
    
    while(1) {
        
				float x = Potentiometer_GetSpeed();
			LCD_command(LCD_CLEAR);
	LCD_set_cursor(0, 0);
	LCD_write_string("Veolcity: ");
	LCD_print_int(x);
	delay_ms(100);
			
    }
}
