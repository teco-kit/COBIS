/**
* board file for the breakout board 192
*
* Author: krohn@teco.edu
* Date: 2004-04-13
*
*/


#ifndef __BROUT180_C__
#define	__BROUT180_C__					001

/**
* Initializes the SSimp
*/
void BreakOutInit();


void BreakOutInit()
{

	// number of analog pins on the connector [0..4]
	// 0: CONN_11..CONN_14 are digital
	// 1: CONN_11 is analog, CONN_12..CONN_14 are digital
	// 2: CONN_11..CONN_12 are analog, CONN_13..CONN_14 are digital
	// 3: CONN_11..CONN_13 are analog, CONN_14 is digital
	// 4: CONN_11..CONN_14 are analog

	// set analog pins
	PCSetAnalogPorts(ANALOG_CONN_11_12_13_14);

	// set all digital pins to input

	bit_set(TRIS_CONN_02);
	bit_set(TRIS_CONN_03);
	bit_set(TRIS_CONN_04);
	bit_set(TRIS_CONN_05);
	bit_set(TRIS_CONN_06);
	bit_set(TRIS_CONN_07);
	bit_set(TRIS_CONN_08);
	bit_set(TRIS_CONN_09);
	bit_set(TRIS_CONN_11);
	bit_set(TRIS_CONN_12);
	bit_set(TRIS_CONN_13);
	bit_set(TRIS_CONN_14);
	bit_set(TRIS_CONN_15);
	bit_set(TRIS_CONN_16);
	bit_set(TRIS_CONN_17);
	bit_set(TRIS_CONN_18);
	bit_set(TRIS_CONN_19);
}

#endif // __brout