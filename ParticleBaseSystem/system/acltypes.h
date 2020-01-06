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


/**
* This File describes the standard naming convention for type declaration used within Particle packets.
*
* This file is also used as header file included into application programs, where the defined declarations
* are applied.
*
* Important note: The defined types are mandatory!!!
*
* TecO, University of Karlsruhe, Germany
* http://www.teco.edu
* 2003/07/16
*
*	changes:
*	-------
*
*			2003/07/17	albert: control types for remote shutdown added
*
*
*
* version 002 (cdecker, 2004-04-08)
* - added APS type for the configurable sensor nodes
* - added CRS type for the configurable sensor nodes
* - added CET type for EEPROM test on SPart
*
*
* version 003 (schmidtk, 2004-08-02)
* - changed CRR type for shutdown particle (look below where type is declared)
*
*
* version 004 (sabin, 2004-10-27)
* - added CLE type (control leds).
* - added CJC type (SDJS configuration).
* - added CJR type (request for SDJS results).
* - added CJA type (answer to request for SDJS results).
* - added SJS type (SDJS start type).
*
* version 005 (sabin, 2005-02-07)
* - added CPI type (set production id)
*
* version 006 (sabin, 2004-02-08)
* - changed CFS type (control fieldstrength)
* - added CSF type (control fieldstrength answer)
* - added CIC type (configure rssi)
* - added CIR type (request rssi results)
* - added CIA type (answer rssi results)
* - added SIS type (send rssi packets)
*
**/


#define __ACLTYPES_H__			003

#define ACL_TYPE_ALPHA(A,B,C) ((A-0x33)+((B-0x33)*40)+((C-0x33)*1600))

#define ACL_TYPE_ALPHA_HI(A,B,C) (ACL_TYPE_ALPHA(A,B,C)>>8)
#define ACL_TYPE_ALPHA_LO(A,B,C) (ACL_TYPE_ALPHA(A,B,C)&0xff)


#define ACL_TYPE_ALPHA_ARG(A,B,C) ACL_TYPE_ALPHA_HI(A,B,C),ACL_TYPE_ALPHA_LO(A,B,C)

// top level characters (first character of the three)
/**

A	for Artefact
R	Raw data, no context
C	Control Channel: Exchange of control messages (low level)between nodes
S	Sensors/Actuators

**/




// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------- S: Raw Sensor values -------------------------------------------------------------
// index always means the index of the sensor on on artefact.
// Sometimes there are more than one sensor of the same type: e.g. light level from different directions and so on


// Sensor Gravity
// 		Data:
//			- signed 16bit: value. (Unit: 1g = value 100)
//			- 1 unsigned byte index
// x Axis
#define ACL_TYPE_SGX_HI	234
#define ACL_TYPE_SGX_LO	128
// y Axis
#define ACL_TYPE_SGY_HI 240
#define ACL_TYPE_SGY_LO 192
// z Axis
#define ACL_TYPE_SGZ_HI 247
#define ACL_TYPE_SGZ_LO 0



// Sensor Light level:
//		Data:
//			- 1 unsigned byte is type of sensor(25:TSL250,26:TSL260)
//			- 1 unsigned byte is light level, 1 unsigned
//			- 1 unsigned byte index
#define ACL_TYPE_SLI_HI 141
#define ACL_TYPE_SLI_LO 136



// Sensor Temperature level
//		Data:
//			- 1 signed byte for celsius degree
//			- 1 unsigend byte index
#define ACL_TYPE_STE_HI 117
#define ACL_TYPE_STE_LO 200



// Sensor Force:
//		Data:
//			- 1 unsigned byte for force value
//			- 1 unsigned byte for index
#define ACL_TYPE_SFC_HI 103
#define ACL_TYPE_SFC_LO 24


// Sensor Audio
// 		Data:
//			- 1 unsigned byte for type of measurement (1: mean volume)
//			- n byte value	(for type 1(volume): 1 unsigned byte volume level)
//			- 1 unsigned byte index
#define ACL_TYPE_SAU_HI 214
#define ACL_TYPE_SAU_LO 208


// Sensor Switch
// 		Data:
//			- 1 unsigned byte for type of measurement (1: on/off, 2:no of turn is given time)
//			- 1 unsigned byte value	(for type 1(on/off): 1 unsigned byte  (1:on; 0: off)
//									 for type 2(turns):  1 unsigned byte no of turns)
//			- 1 unsigned byte index
#define ACL_TYPE_SSW_HI 230
#define ACL_TYPE_SSW_LO 32

// Sensor Voltage
// 		Data:
//			- 1 unsigned 16bit value ( Unit: millivolt (mV) )
#define ACL_TYPE_SVC_HI 105
#define ACL_TYPE_SVC_LO 152


// SDJS start type.
// starts an sdjs slot.
#define ACL_TYPE_SJS_HI		203
#define ACL_TYPE_SJS_LO		184

// RSSI send packet.
#define ACL_TYPE_SIS_HI		203
#define ACL_TYPE_SIS_LO		144



// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------- A: Artefacts -------------------------------------------------------------


// Control and Management Artefact (a virtual type that always stands in front of all control and management packets
// 		Data:
//			- n byte control data
#define ACL_TYPE_ACM_HI	165
#define ACL_TYPE_ACM_LO	14


// Mediacup
//		Data:
//			- 1 byte classification (version, index, ...)
#define ACL_TYPE_AMC_HI 104
#define ACL_TYPE_AMC_LO	30


// SmartIts
//		Data:
//			- 1 byte classification (version, index, ...)
#define ACL_TYPE_ASM_HI	167
#define ACL_TYPE_ASM_LO	142


// Chair
//		Data:
//			- 1 byte classification (version, index, ...)
#define ACL_TYPE_ACH_HI	133
#define ACL_TYPE_ACH_LO	206


// Desk
//		Data:
//			- 1 byte classification (version, index, ...)
#define ACL_TYPE_ADE_HI	115
#define ACL_TYPE_ADE_LO	54


// DoorPlate
//		Data:
//			- 1 byte classification (version, index, ...)
#define ACL_TYPE_ADP_HI	183
#define ACL_TYPE_ADP_LO	246


// Friends
//		Data:
//			- 1 byte classification (version, index, ...)
#define ACL_TYPE_AFR_HI	196
#define ACL_TYPE_AFR_LO	198

// MemoClip
//		Data:
//			- 1 byte classification (version, index, ...)
#define ACL_TYPE_AMM_HI	166
#define ACL_TYPE_AMM_LO	158


// Door
//		Data:
//			- 1 byte classification (version, index, ...)
#define ACL_TYPE_ADO_HI	177
#define ACL_TYPE_ADO_LO	182


// Batch
//		Data:
//			- 1 byte classification (version, index, ...)
#define ACL_TYPE_ABA_HI	89
#define ACL_TYPE_ABA_LO	230


// LocationBeacon
//		Data:
//			- 1 byte classification (version, index, ...)
//			- n byte location
#define ACL_TYPE_ALB_HI	97
#define ACL_TYPE_ALB_LO	182


// Context Label
//		Data:
//			- 1 byte classification (version, index, ...)
#define ACL_TYPE_ACL_HI	158
#define ACL_TYPE_ACL_LO	206


// Shaker
//		Data:
//			- 1 byte classification (version, index, ...)
//			- 1 byte no of shakes
#define ACL_TYPE_ASH_HI	136
#define ACL_TYPE_ASH_LO	78


// Relate artefacts
//		Data:
//			- 1 byte classification (version, index, ...)
#define ACL_TYPE_ARE_HI	117
#define ACL_TYPE_ARE_LO	102



// Artefact Alchemist, for "context nuggets"

#define ACL_TYPE_AAL_HI			158		//artefact alchemist
#define ACL_TYPE_AAL_LO			126


// Artefact Programmable Sensor, for configurable sensor nodes
// no data
#define ACL_TYPE_APS_HI		204
#define ACL_TYPE_APS_LO		150



// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------- R: raw data ----------------------------------------------------------------------

// Raw Data (free for any use)
//		Data:
//			- n byte raw data
#define ACL_TYPE_RDA_HI	90
#define ACL_TYPE_RDA_LO	71


// Raw Data Extendable for further (free for any use)
//		Data:
//			- 1 byte classification (version, index, ...)
//			- n byte further classification or data
#define ACL_TYPE_RDX_HI	234
#define ACL_TYPE_RDX_LO	7








// -------------------------------------------------------------------------------------------------------------------------
// -------------------------------------- C: control types----------------------------------------------------------------------


// Announcement
//		Data:
//			- n byte further classification or data
#define ACL_TYPE_CAN_HI	171
#define ACL_TYPE_CAN_LO	0



// Helo (asks a remote artefact to answer with oleh)
//		Data:
//			- 8 byte destination ID (multicast, broadcast allowed)
#define ACL_TYPE_CHE_HI	115
#define ACL_TYPE_CHE_LO 216


// Oleh (answer to Helo)
//		Data:
//			- 8 byte originator ID
#define ACL_TYPE_CEH_HI	134
#define ACL_TYPE_CEH_LO 32

// sync rate set (set the sync rate)
//		Data:
//			- 8 byte destination ID (multicast, broadcast allowed)
//			- 1 unsigned byte rate (0: never sync; 255: always sync)
#define ACL_TYPE_CSR_HI	198
#define ACL_TYPE_CSR_LO 208


// program burn (over the air programming)
//		Data:
//			- 8 byte destination ID (multicast, broadcast allowed?)
//			- n byte data
#define ACL_TYPE_CPR_HI	198
#define ACL_TYPE_CPR_LO 88


// remote control (for future particle telnet etc.)
//		Data:
//			- 1 byte version
//			- 8 byte destination ID (multicast, broadcast allowed?)
//			- n byte data
#define ACL_TYPE_CRT_HI	211
#define ACL_TYPE_CRT_LO 40


// remote reset and remote shut down
//		Data:
//			- 8 byte destination ID (multicast, broadcast allowed)
//			- 2 byte shutdown code (0 0 = reset; 255 255 = shutdown forever; otherwise first byte minutes, second byte seconds; max. time: 255min 254sec)
#define ACL_TYPE_CRR_HI	198
#define ACL_TYPE_CRR_LO 168


// field strength regulation (set the fieldstrength in remote artefacts)
// send addressed!
//		Data:
//			- 1 byte value
#define ACL_TYPE_CFS_HI	203
#define ACL_TYPE_CFS_LO 8


// answer to field strength regulation (indicate successful fieldstrength change)
//		Data:
///			- 1 byte value
//			- 8 byte destination ID
#define ACL_TYPE_CSF_HI	123
#define ACL_TYPE_CSF_LO 208


// set new ID (only with extra hardware...)
//		Data:
//			- 8 byte new ID
#define ACL_TYPE_CID_HI	109
#define ACL_TYPE_CID_LO 192


// selft test result
//		Data:
//			- n byte results
#define ACL_TYPE_CST_HI	211
#define ACL_TYPE_CST_LO 80


// address control field
//		Data:
//			- 8 byte adress
#define ACL_TYPE_CAD_HI			108		//adress control sequence
#define ACL_TYPE_CAD_LO			128


// address control field
//		Data:
//			- 1 byte type (0: request for acknowledge, 1 ackknowledge packet)
//			- 1 byte ack sequence number
//
#define ACL_TYPE_CAC_HI 102
#define ACL_TYPE_CAC_LO 64


// timestamp request
//		Data: 0 bytes
//
#define ACL_TYPE_CTR_HI 198
#define ACL_TYPE_CTR_LO 248

// time answer - time originated from a time server
//		Data:
//			- 1 byte year
//			- 2 byte month
//			- 3 byte day in month
//			- 4 byte hour
//			- 5 byte min
//			- 6 byte sec
//			- 7 byte 0 (reserved)
//			- 8 byte 0 (reseverd)
//
#define ACL_TYPE_CTA_HI 92
#define ACL_TYPE_CTA_LO 184


// timestamp - time originated from a particle
//		Data:
//			- 1 byte year
//			- 2 byte month
//			- 3 byte day in month
//			- 4 byte hour
//			- 5 byte min
//			- 6 byte sec
//			- 7 byte 0 (reserved)
//			- 8 byte 0 (reseverd)
//
#define ACL_TYPE_CTS_HI 205
#define ACL_TYPE_CTS_LO 56



// EEPROM Test on SPart
//		Data:
//			- 1 byte test result (one of 0..9)
//				0 means successful, otherwise test failed
//
#define ACL_TYPE_CET_HI		209
#define ACL_TYPE_CET_LO		 32

// Control Remote Sensor
// CRS | sensor-id | rate | sensor-id |rate etc.
// where sensor-id is
//	0			 adxl210
//  1			 adxl210_z
//  2			 SP0101NC1
//	3			 tsl250_A
//	4			 tsl2550
//  5			 fsr
//  6			 tc74
//  7			 voltage-sensor from coreboard
//  8			 tsl250_B
//  9			 rtc
//  10			 sparteeprom
//
//  where rate is n, meaning sending sensor values every 2^n slots.
//  values from 0 (every slot) to 15 (every 32768th slot).
#define ACL_TYPE_CRS_HI		204
#define ACL_TYPE_CRS_LO		232


// Control Leds
// Data:
//	1st byte 	ledstyleLEDS_NORMAL		1
//					LEDS_OFF			0		//leds off and stack doesnt touch them
//					LEDS_ON_SEND		2
// 					LEDS_ON_RECEIVE		3
//					LEDS_ONLY_ONE		5
//					LEDS_ON_CRC_ERROR	6
// 	2nd byte	red led
//					on					1
//					off					0
//	3rd byte	blue led
//					on					1
//					off					0
#define ACL_TYPE_CLE_HI		116
#define ACL_TYPE_CLE_LO		120



// Configures SDJS. (See SDJS_docu).
// Data:
// - type (0: sdjs switched off, 1: now implemented sdjs-version).
// - evoke (0: doesn't evoke sdjs, x>0: evokes sdjs every x slot).
// - led (0: no sdjs-led, 1: switches on led starting sdjs and receiving new configuration, 2: rise pin_d5 when doing sdjs, 3: 1 and 2 (led and pin).
// - symbol (the symbol sent in a sdjs-slot).
// - fieldstrength
// (and optionally:)
// - probability_1 (probability between 0 and 100 the following value is sent with).
// - value_1_min (first value is chosen between value_1_min and value_1_max, if a fixed value is wanted, please put it as min and as max).
// - value_1_max
// - probability_2
// - value_2 (fixed sdjs value).
// - probability_3
// - value_3
// - probability_4
// - value_4
// - value_5 (fixed sdjs value. for the last three possible sdjs-values there's no probability available).
// - value_6
// - value_7
#define ACL_TYPE_CJC_HI		103
#define ACL_TYPE_CJC_LO		168


// Request for SDJS results.
// Data:
// 	- result wanted.
// 		0: send own SDJS configuration.
// 		1: send number of bytes found during last SDJS.
// 		2: send the last SDJS buffer received. Attention: As the answer needs six bytes of types, only the 58 first of 64 bytes in buffer will be sent.
// 		3: wants three more parameters and sends results analogously to SDJSConfigureBigSlots() (see docu)
// 	(and in case the first byte is 3)
//	- number of big slots representing values.
// 	- number of random slots for each big slot (each value).
// 	- number of bits that have to be set in order to count a byte.
#define ACL_TYPE_CJR_HI		197
#define ACL_TYPE_CJR_LO		104

// Answer to request for SDJS results.
// Data:
// 	- result wanted.
//	- results, dependent on type of result wanted:
// 		0: 17 SDJS configuration bytes.
//		1: 1 byte indicating how many bytes set in last sdjs slot.
//		2: 57 first bytes of last sdjs slot.
//		3: 'number of big slots' values, representing number of byte set in the respective big slot in the last sdjs process.
#define ACL_TYPE_CJA_HI		91
#define ACL_TYPE_CJA_LO		40


// Set Production ID. 7 & 9 bit = 2 byte. first 7 bit: class (e.g. 1 for 'particle'), then 9 bit: type (e.g. 229 (version))
// Data:
// 	- 7bit of class & 1bit (most significant) of type.
//	- 8bit (least significant) of type.
#define ACL_TYPE_CPI_HI		142
#define ACL_TYPE_CPI_LO		24


// Configures rssi. (See rssi docu).
// Data:
// - send (0: sending rssi switched off, x!0: every sent packet is followed by rssi signal).
// - receive (0: receiving rssi switched off, x!0: every rssi signal is measured).
// - continuity of result (0: results are sent only on request, x!0: results sent continuously)
// - ID (0: results are sent without id of measured packet, x!0: results sent with id of measured packet).
// - led (0: no leds, x!0: blue led is switched on during rssi measurement.)
// - pin (0: no pins, x!0: PIN_D5 respectively PIN_CONN_18 is high during every adc-measurement.
#define ACL_TYPE_CIC_HI		103
#define ACL_TYPE_CIC_LO		128


// Request for rssi results.
// Data:
// 	- difference ( x!0: difference between zero average level and one average level is added to results.)
// 	- average zero: ( x!0: zero average level is added to results.)
// 	- average one: ( x!0: one average level is added to results.)
// 	- samples zero: ( x!0: 15 or 30 (depending on space) zero samples are added to results.)
// 	- samples one: ( x!0: 15 or 30 (depending on space) one samples are added to results.)
#define ACL_TYPE_CIR_HI		197
#define ACL_TYPE_CIR_LO		64

// Answer to request for rssi results.
// Data:
// 	- new value? (0: no new measurement after the last request, 1: results from new measurement.)
//	- results wanted. (see request for rssi results.)
//	- id of measured packet, if wanted. (see rssi config.)
#define ACL_TYPE_CIA_HI		91
#define ACL_TYPE_CIA_LO		0
