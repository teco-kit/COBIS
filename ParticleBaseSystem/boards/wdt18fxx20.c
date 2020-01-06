/**
  * Copyright (c) 2004, Telecooperation Office (TecO),
  * Universitaet Karlsruhe (TH), Germany.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions
  * are met:
  *
  *   * Redistributions of source code must retain the above copyright
  *     notice, this list of conditions and the following disclaimer.
  *   * Redistributions in binary form must reproduce the above
  *     copyright notice, this list of conditions and the following
  *     disclaimer in the documentation and/or other materials provided
  *     with the distribution.
  *   * Neither the name of the Universitaet Karlsruhe (TH) nor the names
  *     of its contributors may be used to endorse or promote products
  *     derived from this software without specific prior written
  *     permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  */


/* Software-WatchDogTimer-Driver
*
* You can use this driver to use the software-watchdog in your program giving it a rate
* Be careful: You have to disable the hardware-watchdog in the #fuses in the header-file
* of your program (#fuses nowdt)
* software watchdog is significant only if hardware watchdog is turned off!!
*
*
* author: Klaus Schmidt (schmidtk@teco.edu)
* date: 2004-05-06
* ver: 001
*
* changed 2004-04-19 by sabin (wendhack@teco.edu)
* - inserted PIC_TBLPTRU = 0 in WDTConfig, otherwise stack doesn't work afterwards.
* - check for processor before WDTConfig.
* - WDT_DEFAULT to determine rate in case of nonsense.
*
* ver: 002
* changed 2004-05-26 by klaus (schmidtk@teco.edu)
* - inserted proto-type-functions for ACL, because of using Software-WDT in STACK for shutdown
*
* ver: 002b
* changed 2004-07-13 by klaus (schmidtk@teco.edu)
* - deleted proto-type-functions for ACL and inserted disabling and enabling interrupts depended
*	on whether you are in an interrupt or not
* - added processor examination
*
* ver: 003
* changed 2004-08-03 by klaus (schmidtk@teco.edu)
* - added WDTTest() to check, if Software-WDT is enabled and WDTReadValue() to read out the value of WDT-Postscaler
*/
#ifndef __WDT18FXX20_C__
#define __WDT18FXX20_C__	003

// Examination of the right processor

#ifdef 		  __18f6620_C__ #define VALID_PROCESSOR	1
#else #ifdef  __18f6720_C__ #define VALID_PROCESSOR	1
	  #else 				#define INVALID_PROCESSOR 1
	  #endif
#endif

#define WDT_DEFAULT 	0	// for values > 7. values see below.


#ifdef VALID_PROCESSOR

void WDTConfig(unsigned int timeout) {
// same code for 18f6620 and 18f6720

	unsigned int wdtrate;
	unsigned int global_interrupt_flag;

		if( timeout > 7 ) timeout = WDT_DEFAULT;
		wdtrate = (( timeout << 1 ) & 0b11111110 );	// attention: important to clear low bit of wdtrate, because this switches on hardware watchdog, meaning the software routines below become useless.

		WDTDisable();

		global_interrupt_flag = bit_test(PIC_INCON,7);	// important for using WDTConfig during Interrupt in Stack when you
		if (global_interrupt_flag)						// are in an interrupt; you may not disable interrupts GLOBAL because
		{												// this disables the peripheral interrupts too and you need the timer1
		disable_interrupts( GLOBAL );					// interrupt to guarantee proper working of your main program after interrupt
		}												// important to disable interrupts in order to not interrupt writing to configuration bits
														// if in future the priorities of interrupts are used, you have to imnplement this
														// function mode otherwise

		PIC_TBLPTRL = 3;					// write 3bytes address in tablepointer.
		PIC_TBLPTRH = 0;
		PIC_TBLPTRU = 48;

		PIC_TABLAT = wdtrate;				// write value in tablewrite source register.

		#asm
		TBLWT*;								// command to write value in source register to address.
		#endasm

		bit_set( PIC_EECON1, 7 );			// security bits to set/ values to write because addressing configuration bits.
		bit_set( PIC_EECON1, 6 );
		bit_set( PIC_EECON1, 2 );

		PIC_EECON2 = 0x55;
		PIC_EECON2 = 0xAA;

		bit_set( PIC_EECON1, 1 );			// now write is performed.

		#asm
		nop									// 3 nops necessary after writing configuration bits.
		nop
		nop
		#endasm

		PIC_TBLPTRU = 0;					// magic line: without this the stack doesn't recover after WDTConfig. Up to now there's no logical explanation for this....
		bit_clear( PIC_EECON1, 2);

		if (global_interrupt_flag)						// important for using WDTConfig during Interrupt in Stack
		{												// when you are in an interrupt, you may not enable interrupts
		enable_interrupts( GLOBAL );					// but when you are not in an interrupt, you have to enable interrupts
		}												// to guarantee the proper work of your main program
}

#else
	#error NOT DEFINED A VALID PROCESSOR FOR USING THIS FUNCTION
#endif

void WDTEnable() {
	bit_set( PIC_WDTCON, 0 );
}

void WDTDisable() {
	bit_clear( PIC_WDTCON, 0 );
}

unsigned int WDTTest() {
	unsigned int test;

	test = bit_test( PIC_WDTCON, 0);
	return test;
}

unsigned int WDTReadValue() {
	unsigned int value;
		PIC_TBLPTRL = 3;					// write 3bytes address in tablepointer.
		PIC_TBLPTRH = 0;
		PIC_TBLPTRU = 48;
		#asm
		TBLRD*;								// read the data from adress saved in tablepointer
		#endasm
	value = PIC_TABLAT;
		PIC_TBLPTRU = 0;					// magic line: without this the stack doesn't recover after WDTConfig. Up to now there's no logical explanation for this....
	value >>= 1;
	return value;
}

#endif