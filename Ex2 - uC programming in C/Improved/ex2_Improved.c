////////////////////////////////////
// EXERCISE 2 - IMPROVED, TDT4258 //
////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"	// List of useful memory mapped registers
#include "timer.h"
#include "dac.h"
#include "gpio.h"
#include "sound.h"
#include "sound_samples.h"
#include "interrupt_handlers.h"

// "volatile" variables are omitted from optimization because their values can be changed by code outside the scope of current file at any time
volatile uint16_t period;
volatile int8_t volume;
Sound *current_Sound, Melody, Tone1, Tone2, Tone3;		// Sounds (Melody and tones) definition and initialization
volatile uint32_t smpl;
volatile uint32_t pre_buttons;

// Declaration of peripheral setup (prototype) functions
void save_energy_ON();		// Saving energy techniques activated
void save_energy_OFF();	// Saving energy techniques deactivated

// Some techniques to reduce the energy consumption
void save_energy_ON() {
	*SCR = 0x6;					// Enables deep sleep (bit SLEEPDEEP) and automatic sleep on return from interrupt handler (bit SLEEPONEXIT)
	//*EMU_MEMCTRL = 0x7;		// Disabling unused RAM blocks. Mode BLK123 to power down RAM blocks 1-3 (bits POWERDOWN)
	//*CMU_HFCORECLKDIV = 0x9;	// Reducing frequency of High Frequency Core Clock 512 times (smallest) in respect of main High Frequency Clock (HFCLK)
}

void save_energy_OFF() {
	*SCR = 0x0;
}

int main(void) {
	period = SAMPLING_PERIOD;
	volume = VOLUME_DEF;
	smpl = 0;
	pre_buttons = 0;

	Melody.data = data_Melody;
	Melody.length = LENGTH_MELODY;

	Tone1.data = data_Tone1;
	Tone1.length = LENGTH_TONE1;

	Tone2.data = data_Tone2;
	Tone2.length = LENGTH_TONE2;

	Tone3.data = data_Tone3;
	Tone3.length = LENGTH_TONE3;
	
	// Peripheral setup functions
	setupTimer1();
	setupDAC0();
	setupGPIO();

	// Enable interrupt handling
	setupNVIC();

	// For higher energy efficiency, sleep while waiting for interrupts instead of infinite loop for busy-waiting
	save_energy_ON();
	
	// Melody reproduce at start-up
	playAudio(&Melody);

	//Wait for Interrupt instruction (from assembler)
	__asm("wfi");

	return 0;
}

/* If other interrupt handlers are needed, use the following names:
 - NMI_Handler			- HardFault_Handler		- MemManage_Handler		- BusFault_Handler		- UsageFault_Handler
 - Reserved7_Handler	- Reserved8_Handler		- Reserved9_Handler 	- Reserved10_Handler 	- SVC_Handler
 - DebugMon_Handler		- Reserved13_Handler	- PendSV_Handler		- SysTick_Handler		- DMA_IRQHandler
 - GPIO_EVEN_IRQHandler	- GPIO_ODD_IRQHandler	- USART0_RX_IRQHandler	- USART0_TX_IRQHandler	- USB_IRQHandler
 - ACMP0_IRQHandler		- ADC0_IRQHandler		- DAC0_IRQHandler		- I2C0_IRQHandler 		- I2C1_IRQHandler
 - TIMER0_IRQHandler	- TIMER1_IRQHandler		- TIMER2_IRQHandler		- TIMER3_IRQHandler  	- USART1_RX_IRQHandler
 - USART1_TX_IRQHandler	- LESENSE_IRQHandler	- USART2_RX_IRQHandler	- USART2_TX_IRQHandler	- UART0_RX_IRQHandler
 - UART0_TX_IRQHandler  - UART1_RX_IRQHandler	- UART1_TX_IRQHandler	- LEUART0_IRQHandler	- LEUART1_IRQHandler
 - LETIMER0_IRQHandler	- PCNT0_IRQHandler		- PCNT1_IRQHandler		- PCNT2_IRQHandler		- RTC_IRQHandler
 - BURTC_IRQHandler		- CMU_IRQHandler		- VCMP_IRQHandler  		- LCD_IRQHandler		- MSC_IRQHandler
 - AES_IRQHandler		- EBI_IRQHandler		- EMU_IRQHandler */