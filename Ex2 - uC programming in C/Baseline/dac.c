#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"	// List of useful memory mapped registers

// DAC0 configuration (NOTE: Core clock runs at 14 MHz by default)
void setupDAC0() {
	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_DAC0;	// Enable clock of ADC (bit 17) to enable it working
	
	*DAC0_CTRL = 0x50010;	// DAC settings:
							// - All channels with enabled refresh are refreshed every 8 prescaled cycles (REFRSEL - bits 20-21)
							// - Clock prescaled to 14MHz/(2^5) =437.5 KHz (PRESC - bits 16-18)
							// - Internal 1.25 V bandgap reference (REFSEL - bits 8-9)
							// - DAC output to pin enabled. DAC output to ADC and ACMP disabled (OUTMODE - bits 4-5)
							// - DAC is set in continuous mode (CONVMODE - bits 2-3)
							// - Sine mode disabled. Sine reset to 0 degrees (SINEMODE - bit 1)
							// - DAC has single ended output (DIFF - bit 0)

	// Overall, we are using two single ended outputs (with a voltage "VOUT = VDACn_OUTx - VSS= Vref x CHxDATA/4095" each)
}

void startDAC0() {	
	*DAC0_CH0CTRL = 0x01;	// Enable channel 0 of DAC0 (triggered by DAC0_CH0DATA, no PRS [Peripheral Reflex System])
	*DAC0_CH1CTRL = 0x01;	// Enable channel 1 of DAC0 (triggered by DAC0_CH1DATA, no PRS [Peripheral Reflex System])
}

void stopDAC0() {
	*DAC0_CH0CTRL = 0x00;
	*DAC0_CH1CTRL = 0x00;
}