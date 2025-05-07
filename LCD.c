#include "headers.h"

void display(void){
	
	if (!ignition && !ignition_changed){ // car is off
		LCD_command(LCD_CLEAR);
		return;
	}
	
	
	if (ignition_changed){
		LCD_command(LCD_CLEAR);
		if (ignition){

		LCD_set_cursor(0, 0);
    LCD_write_string("IGNITION ON ");
			
			for (int i = 0; i < 11; ++i){
				LCD_set_cursor(1, i);
				LCD_write_string("=");
				delay_ms(500);
			}
			
			
	
	} else {
		
		LCD_set_cursor(0, 0);
    LCD_write_string("IGNITION OFF");
		
		for (int i = 0; i < 12; ++i){
			LCD_set_cursor(1, i);
			LCD_write_string("=");
			delay_ms(500);
		}
		
		LCD_command(LCD_CLEAR);
		LCD_set_cursor(0,0);
		LCD_write_string("Doors Unlocked");
		delay_ms(4000);
		
	}
	
	ignition_changed = false;
	LCD_command(LCD_CLEAR);
}

	// speed
	LCD_set_cursor(0, 14);
	LCD_print_int(speed);
if (speed < 10){
			clear_cell(0, 15);
	}

	return;
}

void DisplayTask(void *pvParameters){
    /* Initialize periodic timing */
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(100); // 100ms refresh rate
    
    xLastWakeTime = xTaskGetTickCount();
	
	while(1){
		
		
		
		
			if (!ignition && !ignition_changed){ // car is off
		LCD_command(LCD_CLEAR);
		continue;
	}
	
	
	if (ignition_changed){
		LCD_command(LCD_CLEAR);
		if (ignition){

		LCD_set_cursor(0, 0);
    LCD_write_string("IGNITION ON ");
			
			for (int i = 0; i < 11; ++i){
				LCD_set_cursor(1, i);
				LCD_write_string("=");
				delay_ms(300);
			}
			
			
	
	} else {
		
		LCD_set_cursor(0, 0);
    LCD_write_string("IGNITION OFF");
		
		for (int i = 0; i < 12; ++i){
			LCD_set_cursor(1, i);
			LCD_write_string("=");
			delay_ms(300);
		}
		
		LCD_command(LCD_CLEAR);
		LCD_set_cursor(0,0);
		LCD_write_string("Doors Unlocked");
		
		delay_ms(3000);
		
	}
	
	ignition_changed = false;
	LCD_command(LCD_CLEAR);
}

	//gear
	LCD_set_cursor(1, 14);
if (drive){
	LCD_write_string("D");
} else if (!drive){
	LCD_write_string("R");
}

	//DOOR
	LCD_set_cursor(0, 1);
if (door_opened){
	LCD_write_string("O");
} else if (!door_opened){
	LCD_write_string("C");
}
	//Lock
	LCD_set_cursor(0, 0);
if (door_locked){
	LCD_write_string("L");
} else if (!door_locked){
	LCD_write_string("U");
}

	// speed
	LCD_set_cursor(0, 14);
	LCD_print_int(speed);
if (speed < 10){
			clear_cell(0, 15);
	}
		

		// distance
		LCD_set_cursor(1, 0);
		LCD_write_string("     ");
		LCD_set_cursor(1, 0);
		LCD_print_int(latest_distance);
		LCD_write_string("cm");



	
	vTaskDelayUntil(&xLastWakeTime, xFrequency);
	
	
	}
	
}



void clear_cell(int c, int r){
	
	LCD_set_cursor(c,r);
	LCD_write_string(" ");
	
}





// I2C initialization and GPIO alternate function configuration
void I2C1_Init(void)
{
    SYSCTL_RCGCGPIO |= 0x00000001;  // Enable the clock for port A
    SYSCTL_RCGCI2C  |= 0x00000002;  // Enable the clock for I2C 1
    
    // Wait for clock stabilization
    while((SYSCTL_PRGPIO & 0x01) == 0);
    while((SYSCTL_PRI2C & 0x02) == 0);
    
    GPIOA_DEN |= 0xC0;              // Enable digital for PA6 and PA7 (0xC0 = 0b11000000)
    
    // Configure Port A pins 6 and 7 as I2C 1
    GPIOA_AFSEL |= 0xC0;            // Enable alternate function for PA6 and PA7
    GPIOA_PCTL &= ~0xFF000000;      // Clear PCTL bits for PA6 and PA7
    GPIOA_PCTL |= 0x33000000;       // Set PCTL to I2C function (3) for PA6 and PA7
    GPIOA_ODR |= 0x80;              // SDA (PA7) pin as open drain
    
    I2C1_MCR = 0x0010;              // Enable I2C 1 master function
    
    /* Configure I2C 1 clock frequency
       (1 + TIME_PERIOD) = SYS_CLK /(2*(SCL_LP + SCL_HP) * I2C_CLK_Freq)
       TIME_PERIOD = 16,000,000/(2(6+4)*100000) - 1 = 7 */
    I2C1_MTPR = 0x07;
}

/* wait until I2C Master module is busy */
/* and if not busy and no error return 0 */
static int I2C_wait_till_done(void)
{
    while(I2C1_MCS & 1);   /* wait until I2C master is not busy */
    return I2C1_MCS & 0xE; /* return I2C error code, 0 if no error*/
}

// Send multiple bytes of data to I2C slave device
char I2C1_Write_Multiple(int slave_address, char slave_memory_address, int bytes_count, char* data)
{   
    char error;
    if (bytes_count <= 0)
        return -1;                  /* no write was performed */
    
    /* send slave address and starting address */
    I2C1_MSA = slave_address << 1;
    I2C1_MDR = slave_memory_address;
    I2C1_MCS = 3;                  /* S-(saddr+w)-ACK-maddr-ACK */

    error = I2C_wait_till_done();   /* wait until write is complete */
    if (error) return error;

    /* send data one byte at a time */
    while (bytes_count > 1)
    {
        I2C1_MDR = *data++;         /* write the next byte */
        I2C1_MCS = 1;               /* -data-ACK- */
        error = I2C_wait_till_done();
        if (error) return error;
        bytes_count--;
    }
    
    /* send last byte and a STOP */
    I2C1_MDR = *data++;             /* write the last byte */
    I2C1_MCS = 5;                   /* -data-ACK-P */
    error = I2C_wait_till_done();
    while(I2C1_MCS & 0x40);         /* wait until bus is not busy */
    if (error) return error;
    return 0;                        /* no error */
}

/* This function reads from slave memory location of slave address */
/* read address should be specified in the second argument */
/* read: S-(saddr+w)-ACK-maddr-ACK-R-(saddr+r)-ACK-data-ACK-data-ACK-...-data-NACK-P */
char I2C1_read_Multiple(int slave_address, char slave_memory_address, int bytes_count, char* data)
{
    char error;
    
    if (bytes_count <= 0)
        return -1;         /* no read was performed */

    /* send slave address and starting address */
    I2C1_MSA = slave_address << 1;
    I2C1_MDR = slave_memory_address;
    I2C1_MCS = 3;       /* S-(saddr+w)-ACK-maddr-ACK */
    error = I2C_wait_till_done();
    if (error)
        return error;

    /* to change bus from write to read, send restart with slave addr */
    I2C1_MSA = (slave_address << 1) + 1;   /* restart: -R-(saddr+r)-ACK */

    if (bytes_count == 1)             /* if last byte, don't ack */
        I2C1_MCS = 7;              /* -data-NACK-P */
    else                            /* else ack */
        I2C1_MCS = 0xB;            /* -data-ACK- */
    error = I2C_wait_till_done();
    if (error) return error;

    *data++ = I2C1_MDR;            /* store the data received */

    if (--bytes_count == 0)           /* if single byte read, done */
    {
        while(I2C1_MCS & 0x40);    /* wait until bus is not busy */
        return 0;       /* no error */
    }
 
    /* read the rest of the bytes */
    while (bytes_count > 1)
    {
        I2C1_MCS = 9;              /* -data-ACK- */
        error = I2C_wait_till_done();
        if (error) return error;
        bytes_count--;
        *data++ = I2C1_MDR;        /* store data received */
    }

    I2C1_MCS = 5;                  /* -data-NACK-P */
    error = I2C_wait_till_done();
    *data = I2C1_MDR;              /* store data received */
    while(I2C1_MCS & 0x40);        /* wait until bus is not busy */
    
    return 0;       /* no error */
}

// Delay function
void delay_ms(int ms) {
    int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 3180; j++) {} // Approximate value for 1ms delay at 16MHz
}

// Send command to LCD
void LCD_command(unsigned char command) {
    char data_upper, data_lower;
    char data_to_send[4];
    
    data_upper = (command & 0xF0);       // Upper nibble first
    data_lower = ((command << 4) & 0xF0); // Lower nibble second
    
    // Upper nibble with backlight ON and EN=1
    data_to_send[0] = data_upper | LCD_BACKLIGHT | LCD_EN;
    // Upper nibble with backlight ON and EN=0
    data_to_send[1] = data_upper | LCD_BACKLIGHT;
    // Lower nibble with backlight ON and EN=1
    data_to_send[2] = data_lower | LCD_BACKLIGHT | LCD_EN;
    // Lower nibble with backlight ON and EN=0
    data_to_send[3] = data_lower | LCD_BACKLIGHT;
    
    I2C1_Write_Multiple(LCD_ADDR, 0, 4, data_to_send);
    delay_ms(2);  // Commands need > 37us delay
}

// Send data to LCD
void LCD_data(unsigned char data) {
    char data_upper, data_lower;
    char data_to_send[4];
    
    data_upper = (data & 0xF0);       // Upper nibble first
    data_lower = ((data << 4) & 0xF0); // Lower nibble second
    
    // Upper nibble with backlight ON, RS=1, and EN=1
    data_to_send[0] = data_upper | LCD_BACKLIGHT | LCD_RS | LCD_EN;
    // Upper nibble with backlight ON, RS=1, and EN=0
    data_to_send[1] = data_upper | LCD_BACKLIGHT | LCD_RS;
    // Lower nibble with backlight ON, RS=1, and EN=1
    data_to_send[2] = data_lower | LCD_BACKLIGHT | LCD_RS | LCD_EN;
    // Lower nibble with backlight ON, RS=1, and EN=0
    data_to_send[3] = data_lower | LCD_BACKLIGHT | LCD_RS;
    
    I2C1_Write_Multiple(LCD_ADDR, 0, 4, data_to_send);
    delay_ms(1);  // Data needs > 37us delay
}

// Initialize LCD
void LCD_init() {
    delay_ms(50);  // Wait for LCD to power up
    
    // Initialize in 4-bit mode according to datasheet
    char init_sequence[12] = {
        0x30 | LCD_BACKLIGHT | LCD_EN,  // 8-bit mode initialization sequence
        0x30 | LCD_BACKLIGHT,
        0x30 | LCD_BACKLIGHT | LCD_EN,
        0x30 | LCD_BACKLIGHT,
        0x30 | LCD_BACKLIGHT | LCD_EN,
        0x30 | LCD_BACKLIGHT,
        0x20 | LCD_BACKLIGHT | LCD_EN,  // Set to 4-bit mode
        0x20 | LCD_BACKLIGHT,
    };
    
    I2C1_Write_Multiple(LCD_ADDR, 0, 8, init_sequence);
    delay_ms(5);
    
    // Now in 4-bit mode, configure display
    LCD_command(LCD_FUNCTION_SET);  // 2 lines, 5x8 font
    LCD_command(LCD_DISPLAY_ON);    // Display on, cursor off, blink off
    LCD_command(LCD_CLEAR);         // Clear display
    delay_ms(2);                    // Clear command needs extra delay
    LCD_command(LCD_ENTRY_MODE);    // Increment cursor position, no display shift
}

// Write string to LCD
void LCD_write_string(char *str) {
    while (*str) {
        LCD_data(*str++);
    }
}

// Set cursor position
void LCD_set_cursor(int row, int col) {
    int row_offsets[] = {0x00, 0x40};  // 1602A has 2 rows with these offsets
    LCD_command(LCD_SET_CURSOR | (col + row_offsets[row]));
}

void LCD_print_int(int value) {
    char buffer[16];
    int i = 0;
    int isNegative = 0;
    
    // Handle negative numbers
    if (value < 0) {
        isNegative = 1;
        value = -value;
    }
    
    // Handle special case for 0
    if (value == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        LCD_write_string(buffer);
        return;
    }
    
    // Convert digits in reverse order
    while (value != 0) {
        buffer[i++] = (value % 10) + '0';
        value /= 10;
    }
    
    // Add negative sign if needed
    if (isNegative) {
        buffer[i++] = '-';
    }
    
    buffer[i] = '\0';
    
    // Reverse the string
    int start = 0;
    int end = i - 1;
    char temp;
    while (start < end) {
        temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
    
    LCD_write_string(buffer);
}