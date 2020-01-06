/**
* header file for the serrial connector board 182
* Author: krohn@teco.edu
* Date: 2004-04-14
*
*
*/


#ifndef __SERCON182_H__
#define	__SERCON182_H__					001


// pin defs for particle
#ifdef __PC202_H__

	#define PIN_SERCON_RX 					PIN_CONN_06
	#define PIN_SERCON_TX					PIN_CONN_07
	#define PIN_SERCON_CTS					PIN_CONN_09
	#define PIN_SERCON_RTS					PIN_CONN_08

	#define TRIS_SERCON_RX 					TRIS_CONN_06
	#define TRIS_SERCON_TX					TRIS_CONN_07
	#define TRIS_SERCON_CTS					TRIS_CONN_09
	#define TRIS_SERCON_RTS					TRIS_CONN_08


#endif


//pin defs for sensor board
#ifdef __SPART202_H__

	#define PIN_SERCON_RX 					PIN_CONN_19
	#define PIN_SERCON_TX					PIN_CONN_20
	#define PIN_SERCON_CTS					PIN_CONN_09
	#define PIN_SERCON_RTS					PIN_CONN_08

	#define TRIS_SERCON_RX 					TRIS_CONN_19
	#define TRIS_SERCON_TX					TRIS_CONN_20
	#define TRIS_SERCON_CTS					TRIS_CONN_09
	#define TRIS_SERCON_RTS					TRIS_CONN_08

#endif


#endif
