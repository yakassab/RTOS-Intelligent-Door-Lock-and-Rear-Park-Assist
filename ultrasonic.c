#include "headers.h"

#define TRIG_PIN  (1 << 0)  // PB0
#define ECHO_PIN  (1 << 3)  // PB3


uint32_t latest_distance = 0;

static void GPIO_Init(void);
static void Timer1A_Init(void);
static uint32_t get_pulse_duration(void);
static void delayUs(int us);
static void delayMs(int ms);


void Ultrasonic_Init(void) {
    GPIO_Init();
    Timer1A_Init();

}

uint32_t Ultrasonic_GetDistance(void) {
        uint32_t pulse_width = get_pulse_duration();
        uint32_t time_us = pulse_width / 16;
        //latest_distance = ((100-((time_us / 58)-4628100))) * 14 / 50;
			  latest_distance = ((100-((time_us / 58)-4628100)))* 31 / 100;
	return latest_distance;
}

static void GPIO_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x02;            // Enable clock for PORTB
    while((SYSCTL_PRGPIO_R & 0x02) == 0); // Wait until ready

    GPIO_PORTB_DIR_R |= TRIG_PIN;         // PB6 as output (TRIG)
    GPIO_PORTB_DEN_R |= TRIG_PIN;

    GPIO_PORTB_DIR_R &= ~ECHO_PIN;        // PB7 as input (ECHO)
    GPIO_PORTB_DEN_R |= ECHO_PIN;
}

static void Timer1A_Init(void) {
    SYSCTL_RCGCTIMER_R |= 0x02;           // Enable Timer1
    TIMER1_CTL_R = 0;                     // Disable timer during setup
    TIMER1_CFG_R = 0x00;                  // 32-bit mode
    TIMER1_TAMR_R = 0x02;                 // Capture mode, edge-time, up-count
    TIMER1_CTL_R |= 0x0C;                 // Capture both edges
    TIMER1_ICR_R = 0x04;                  // Clear capture flag
    TIMER1_IMR_R = 0x04;                  // Enable capture interrupt
    TIMER1_CTL_R |= 0x01;                 // Enable timer
}

static uint32_t get_pulse_duration(void) {
	
	
    GPIO_PORTB_DATA_R &= ~TRIG_PIN;
    delayUs(2);
    GPIO_PORTB_DATA_R |= TRIG_PIN;
    delayUs(10);
    GPIO_PORTB_DATA_R &= ~TRIG_PIN;

    while((GPIO_PORTB_DATA_R & ECHO_PIN) == 0);
    TIMER1_CTL_R |= 0x01;
    TIMER1_TAV_R = 0;
    while((GPIO_PORTB_DATA_R & ECHO_PIN) != 0);
    uint32_t duration = TIMER1_TAV_R;
    return duration;
}

//void vUltrasonicTask(void *pvParameters) {
//    (void)pvParameters;

//    for (;;) {
//        uint32_t pulse_width = get_pulse_duration();
//        uint32_t time_us = pulse_width / 16;
//        //latest_distance = ((100-((time_us / 58)-4628100))) * 14 / 50;
//			  latest_distance = ((100-((time_us / 58)-4628100)))* 31 / 100;
//				 
//			
//        vTaskDelay(pdMS_TO_TICKS(50));
//    }
//}

static void delayUs(int us) {
    int i;
    for(i = 0; i < us * 4; i++); // Approximate for 16 MHz
}

static void delayMs(int ms) {
    int i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 3180; j++);
} 