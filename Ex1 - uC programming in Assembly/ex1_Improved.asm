////////////////////////////////////
// EXERCISE 1 - BASELINE, TDT4258 //
////////////////////////////////////

// Instructions starting with "." are assembler directives. Look at https://sourceware.org/binutils/docs/as/ARM-Directives.html

.syntax unified			// Sets the Instruction Set Syntax as unified ARM/Thumb assembly syntax.
.include "efm32gg.s"	// Various useful I/O addresses and definitions for EFM32GG.

	//////////////////////////////////////////////////////////////////////////////
	// Exception vector table. It contains addresses for all exception handlers //
	//////////////////////////////////////////////////////////////////////////////

	.section .vectors
		.long	stack_top			/* Top of Stack */
		.long	reset_handler		/* Reset Handler */
		.long	dummy_handler		/* NMI Handler */
		.long	dummy_handler		/* Hard Fault Handler */
		.long	dummy_handler		/* MPU Fault Handler */
		.long	dummy_handler		/* Bus Fault Handler */
		.long	dummy_handler		/* Usage Fault Handler */
		.long	dummy_handler		/* Reserved */
		.long	dummy_handler		/* Reserved */
		.long	dummy_handler		/* Reserved */
		.long	dummy_handler		/* Reserved */
		.long	dummy_handler		/* SVCall Handler */
		.long	dummy_handler		/* Debug Monitor Handler */
		.long	dummy_handler		/* Reserved */
		.long	dummy_handler		/* PendSV Handler */
		.long	dummy_handler		/* SysTick Handler */

		// External Interrupts
		.long   dummy_handler
		.long   gpio_handler		/* GPIO even handler (even and odd same name) */
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   gpio_handler		/* GPIO odd handler (even and odd same name) */
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler
		.long   dummy_handler


	.section .text
		////////////////////////////////////////////////////////////
		// Reset handler. CPU starts executing here after a reset //
		////////////////////////////////////////////////////////////
		.globl  reset_handler
		.type   reset_handler, %function	// ".type" directive sets symbol "reset_handler" to be a function symbol
		.thumb_func			// Specifies the following symbol is the name of a Thumb encoded function
		
		reset_handler:		// Registers to use: r0-r1-r2-r4 for addresses (CMU/PORTA/PORTC/interrupts/saving energy) and r3 for values
			// CMU configuration (Clock enabled for the GPIO pins)
			ldr r0, =CMU_BASE 					// R0 gets the base address of CMU
			ldr r1, [r0, #CMU_HFPERCLKEN0] 		// R3 gets the value in address CMU_BASE + CMU_HFPERCLKEN0
			ldr r2, =0x2000			// R2 set to immediate value 2^13 (so only bit 13 [CMU_HFPERCLKEN0_GPIO] asserted). "mov" only works with values up to 255
			orr r1, r1, r2 						// OR gate to only set position 13 (CMU_HFPERCLKEN0_GPIO) in register CMU_HFPERCLKEN0
			str r1, [r0, #CMU_HFPERCLKEN0] 		// Change memory register CMU_HFPERCLKEN0

			// Configuration of the output GPIOs strength
			ldr r1, =GPIO_PA_BASE			// Load memory address of PORTA (for output [active-low] LEDs)
			mov r3, #0x2					// R3 set to 2 (= high drive strength)
			str r3, [r1, #GPIO_CTRL]		// R3 applied to address GPIO_PA_CTRL (GPIO_PA_BASE + GPIO_CTRL). GPIO_CTRL=0x0 because is 1st byte inside GPIO_PA_BASE

			// Configuration of the output pins (PORTA) - NOTE: To write immediate values, use instruction "mov" (up to 255). Otherwise use "ldr"
			ldr r3, =0x55555555 			// Pins 8-15 of PORTA set as output pins, mode 5 (Push-pull with alt. drive strength mode)
			str r3, [r1, #GPIO_MODEH]		// R3 applied to address GPIO_PA_MODEH (GPIO_PA_BASE + GPIO_MODEH)
			
			// Configuration of the input pins (PORTC) - NOTE: To write immediate values, use instruction "mov" (up to 255). Otherwise use "ldr"
			ldr r2, =GPIO_PC_BASE			// Load memory address of PORTC (for input buttons)
			ldr r3, =0x33333333				// Pins 0-7 of PORTC set as input pins, mode 3 (requires writing in GPIO_PC_DOUT for final configuration)
			str r3, [r2, #GPIO_MODEL]		// R3 applied to address GPIO_PC_MODEL (GPIO_PC_BASE + GPIO_MODEL)
			ldr r3, =0xFF					// Enables pull-up resistor (to avoid missreadings) and filter (for noise tolerance) to the previous input pins
			str r3, [r2, #GPIO_DOUT] 		// R3 applied to address GPIO_PC_DOUT (GPIO_PC_BASE + GPIO_DOUT)

			// Setting up pins 0-7 of PORTC (push buttons) as external interrupts 0-7
			ldr r0, =GPIO_BASE				// Load memory of GPIO base address
			ldr r3, =0x22222222				// Pins 0-7 of PORTC set as external interrupt 0-7 (bits EXTIPSELn[0:2], being n every pin)
			str r3, [r0, #GPIO_EXTIPSELL]	// R3 applied to address GPIO_EXTIPSELL (GPIO_BASE + GPIO_EXTIPSELL)
			
			// Configuration and activation of external interrupts 0-7
			ldr r3, =0x7F	 				// For each external interrupt 0-6 (one bit per pin, except PB7)
			str r3, [r0, #GPIO_EXTIRISE]	// R3 applied to address GPIO_EXTIRISE (GPIO_BASE + GPIO_EXTIRISE). Sets interrupt on 1->0 (rising edge) transition
			str r3, [r0, #GPIO_EXTIFALL]	// R3 applied to address GPIO_EXTIFALL (GPIO_BASE + GPIO_EXTIFALL). Sets interrupt on 0->1 (falling edge) transition
			str r3, [r0, #GPIO_IEN]			// R3 applied to address GPIO_IEN (GPIO_BASE + GPIO_IEN). Enables interrupt generation
			
			// Enable of interrupt handling	
			ldr r4, =ISER0			// Load memory address of Interrupt Set-Enable Register (ISER0)
			ldr r3, =0x802			// Enables just both even (bit 1) and odd (bit 11) GPIO interrupts handlers
			str r3, [r4]			// R3 applied to address ISER0

			// Configuration of low energy mode (EM2 -> Deep Sleep Mode)
			ldr r4, =SCR			// Load memory address of System Control Register (SCR)
			mov r3, #0x6 			// Enables deep sleep (bit SLEEPDEEP) and automatic sleep on return from interrupt handler (bit SLEEPONEXIT)
			str r3, [r4] 			// R3 applied to address SCR
			
			// Disabling unused RAM blocks
			ldr r4, =EMU_BASE				// Load memory address of Energy Management Unit (EMU)
			mov r3, #0x7					// Mode BLK123 to power down RAM blocks 1-3 (bits POWERDOWN)
			str r3, [r4, #EMU_MEMCTRL]		// R3 applied to address EMU_MEMCTRL (EMU_BASE + EMU_MEMCTRL)

			// Reducing the frequency of High Frequency Core Clock (HFCORECLK)
			ldr r4, =CMU_BASE 				// R0 gets the base address of CMU
			mov r3, #0x9					// HFCORECLK (which CPU uses) is reduced 512 times (smallest) in respect of main High Frequency Clock (HFCLK)
			str r3, [r4, #CMU_HFCORECLKDIV]	// R3 applied to address CMU_HFCORECLKDIV (CMU_BASE + CMU_HFCORECLKDIV)

			wfi						// Wait For Interrupt (WFI) instruction to go into low energy mode

		////////////////////////////////////////////////////////////////////////////
		// GPIO handler. CPU jumps here when a GPIO interrupt (both even and odd) //
		////////////////////////////////////////////////////////////////////////////
		.thumb_func			// Specifies the following symbol is the name of a Thumb encoded function
		
		gpio_handler:
				ldr r3, [r0, #GPIO_IF] 		// GPIO Interrupt Flag (GPIO_IF) register is read and stored
				str r3, [r0, #GPIO_IFC]		// Whatever interrupt flag was activated, now is cleared with GPIO Interrupt Flag Clear (GPIO_IFC) register
				
				ldr r3, [r2, #GPIO_DIN]		// Register GPIO_PC_DIN read to fetch input button status.
				mov r8, r3					// An input data copy to only save PB7 (two's complement) status.

				mov r5, #0x1				// R5 is used to find the pushed PB (from PB0 to PB6).
				mov r6, #0x1				// R6 is used to count +1 until the pushed PB is found.
				
			check_PB_pushed:
				and r3, r3, #0x7F			// Removed the two's complement input (PB7) as it is managed independently.
				and r7, r3, r5				// In case GPIO inputs value matches R5 (AND!=0)...
				cbz r7, complement_2		// ...then go to check if 2 complement and to update output LEDs.
				
				lsl r5, r5, #0x1			// If GPIO inputs values do not match R5, R5 is moved to the left for next iteration.
				add r6, r6, #0x1			// R6 counter is increased in 1 unit.
				cmp r5, #0x80				// As long as R5 has not been increased 7 times (as we have PB0-PB6 for output display)...
				bne check_PB_pushed			// ...then go back to repeat the process.
				mov r6, #0x0				// If loop was done already 7 times, just activate no led.
				b update_LEDs

			complement_2:
				and r8, r8, #0x80			// For the comparison to be done next, only the PB7 value is saved.
				cmp r8, #0x80				// If PB7 has NOT been pushed...
				beq update_LEDs				// ...then just update LEDs according to R6...
				mvn r6, r6					// ...else make the two's complement of the output value (NOT gate + 1).
				add r6, r6, #0x1

			update_LEDs:
				lsl r6, r6, #8				// Pins 0-7 in PORTC are used as input, and pins 8-15 in PORTA are used as output...
											// ...then, to work with the same bits, the input data must be shifted 8 positions to the left.
				mvn r6, r6					// NOT gate just to have the output in negative logic (active-low LEDs).
				str r6, [r1, #GPIO_DOUT]	// Output leds are updated.

			bx lr		//Branch to return from a external function. "lr" holds the return address
			
			// NOTE: Debugger (GDB), interrupts and sleep modes do not mix well...

		//////////////////////////////////////////////////////////////////////////
		// Dummy handler. Nothing to do for all handlers defined with this name //
		//////////////////////////////////////////////////////////////////////////
		.thumb_func			// Specifies the following symbol is the name of a Thumb encoded function
		
		dummy_handler:  
			b .		// Jump to this instruction again. Infinite loop to avoid code to finish