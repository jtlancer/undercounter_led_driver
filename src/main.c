/*
 * main.c for undercounter led strip avr driver
 *
 * Created: 3/17/2015 
 *  Author: cosmo
 */ 

#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>
#include <avr/pgmspace.h>

#define PWM_PIN PORTB0
#define PWM_DDR DDB0
//#define WIPER_PIN PORTB2

// gamma correction table from adafruit
const uint8_t PROGMEM gammaTable[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

static inline void initADC(void) {
	// Using default of VCC as VREF and PB0 not connected as VREF 
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0);	// ADC clock prescaler 8 for 125Hz at 1MHz
	ADMUX |= (1 << ADLAR);					// left-adjust result, return only 8 bits
	ADMUX |= (1 << MUX0);					// single-ended ADC input on PB2 (pin 7)
	ADCSRA |= (1 << ADEN);					// enable ADC
	ADCSRA |= (1 << ADATE);					// auto-trigger enable
	ADCSRA |= (1 << ADSC);					// start first conversion
}

static inline void initPins(void) {
	DDRB |= (1 << PWM_DDR);
	PORTB |= (1 << PWM_PIN);
}

static inline void initPWM(void) {
	TCCR0B |= (1 << CS01); // PWM frequency ~488Hz with prescaler of 8 at 1MHz and TOP=MAX=255
	TCCR0A |= (1 << WGM00) | (1 << WGM01) | (1 << COM0A1); //fast PWM mode 3 on 0C0A.
}

int main(void)
{	
	initPins();
    initADC();
    initPWM();

    uint8_t brightness = 0;
	
	while(1)
    {
		brightness = pgm_read_byte(&gammaTable[ADCH]); // use potentiometer value to set gamma corrected brightness
		
		if (brightness > 0) {
			TCCR0A &= ~(1 << COM0A0); //clear inverted-mode bit when dutycycle > 0%
			OCR0A = brightness;
		} else {
			TCCR0A |= (1 << COM0A0); //set inverted-mode bit when dutycycle = 0% for full off
			OCR0A = 255;
		}
		
    } // end while(1)
	return (0);	
} // end main



