
#include <stdint.h>
#include <stdbool.h>

// TM4C123GH6PM Register definitions
#define SYSCTL_BASE             0x400FE000
#define GPIO_PORTA_BASE         0x40004000
#define I2C1_BASE               0x40021000

// SYSCTL Register definitions
#define SYSCTL_RCGCGPIO         (*((volatile uint32_t *)(SYSCTL_BASE + 0x608)))
#define SYSCTL_RCGCI2C          (*((volatile uint32_t *)(SYSCTL_BASE + 0x620)))
#define SYSCTL_PRGPIO           (*((volatile uint32_t *)(SYSCTL_BASE + 0xA08)))
#define SYSCTL_PRI2C            (*((volatile uint32_t *)(SYSCTL_BASE + 0xA20)))

// GPIO Port A Register definitions
#define GPIOA_AFSEL             (*((volatile uint32_t *)(GPIO_PORTA_BASE + 0x420)))
#define GPIOA_ODR               (*((volatile uint32_t *)(GPIO_PORTA_BASE + 0x50C)))
#define GPIOA_DEN               (*((volatile uint32_t *)(GPIO_PORTA_BASE + 0x51C)))
#define GPIOA_PCTL              (*((volatile uint32_t *)(GPIO_PORTA_BASE + 0x52C)))

// I2C1 Register definitions
#define I2C1_MSA                (*((volatile uint32_t *)(I2C1_BASE + 0x000)))
#define I2C1_MCS                (*((volatile uint32_t *)(I2C1_BASE + 0x004)))
#define I2C1_MDR                (*((volatile uint32_t *)(I2C1_BASE + 0x008)))
#define I2C1_MTPR               (*((volatile uint32_t *)(I2C1_BASE + 0x00C)))
#define I2C1_MCR                (*((volatile uint32_t *)(I2C1_BASE + 0x020)))

// Function prototypes
void I2C1_Init(void);  
char I2C1_Write_Multiple(int slave_address, char slave_memory_address, int bytes_count, char* data);
char I2C1_read_Multiple(int slave_address, char slave_memory_address, int bytes_count, char* data);
void delay_ms(int ms);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void LCD_write_string(char *str);
void LCD_set_cursor(int row, int col);
void LCD_print_int(int value);

// LCD I2C address - common value is 0x27 for 1602A with I2C backpack
#define LCD_ADDR 0x27

// LCD commands
#define LCD_CLEAR 0x01
#define LCD_HOME 0x02
#define LCD_ENTRY_MODE 0x06
#define LCD_DISPLAY_ON 0x0C
#define LCD_FUNCTION_SET 0x28  // 4-bit mode, 2 lines, 5x8 font
#define LCD_SET_CURSOR 0x80

// For PCF8574 backpack control bits
#define LCD_EN 0x04  // Enable bit
#define LCD_RW 0x02  // Read/Write bit
#define LCD_RS 0x01  // Register select bit
#define LCD_BACKLIGHT 0x08  // Backlight control

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
    
    // Write to first line
    LCD_set_cursor(0, 0);
    LCD_write_string("mia san mia");

//    // Write to second line
    LCD_set_cursor(1, 0);
    LCD_write_string("===========");
    
    while(1) {
        // Main loop
        // You can add code here to update the display periodically if needed
    }
}
