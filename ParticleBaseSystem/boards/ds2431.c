/**
* driver for 1-wire ID chip DS 2431 on the particle 224
*
* Author: krohn@teco.edu
* Date: 2004-08-12
*
* 002: some minor things like return vals
* 003: corrected the readbyte one wire to lsb first
* 004: timings corrected uwe
*
* 005: 	Inserted CRC-check (new: IDChipCheckCRC & CRC_TABLE) of ID (in: IDChipGetId).
*		IDChipGetId returns 0 if problem with OneWire stuff or wrong CRC, 1 if everything ok.
*		2004-11-03 by sabin
*
* 006: 	Inserted IDChipReadEeprom, IDChipWriteEeprom, IDChipCheckCRCScratchpad, IDChipWriteProductionID
*		whereas: IDChipCheckCRCScratchpad doesn't work at all, and IDChipWriteProductionID & IDChipWriteEeprom
*		only work without checking results......... preliminary version.....
*		2004-11-07 by sabin
* 007:	ID fixed. bytes of unique id swopped.
* 		2004-12-15 by sabin (wendhack@teco.edu)
* 008:	- IDChipInit() prototype changed to 'void'.
		- Get production id from board headerfile, see IDChipGetId().
* 		2005-09-29 by sabin (wendhack@teco.edu)
*/


#ifndef __DS2431_C__
#define	__DS2431_C__					008


// macros
#define ID_CHIP_COMM_READ_ROM			0x33
#define	ID_CHIP_COMM_SKIP_ROM			0xCC
#define	ID_CHIP_COMM_READ_MEMORY		0xF0
#define	ID_CHIP_COMM_WRITE_SCRATCHPAD	0x0F
#define	ID_CHIP_COMM_COPY_SCRATCHPAD	0x55

#define ID_CHIP_ADDRESS_HI				0x00
#define ID_CHIP_ES_BYTE					0x07

#define ID_CHIP_PRODUCTION_ID_ADDRESS	0


//functions
void IDChipInit();
void IDChipOn();
void IDChipOff();

#separate
int IDChipGetId(int *data);
int IDChipCheckCRCID( unsigned int* data );
unsigned int IDChipReadEeprom( unsigned int address, unsigned int number_bytes, unsigned int* buff );
unsigned int IDChipWriteEeprom( unsigned int address, unsigned int* buff );
unsigned int IDChipCheckCRCScratchpad( unsigned int* crc, unsigned int address, unsigned int* buff );
unsigned int IDChipWriteProductionID( unsigned int production_id_hi, unsigned int production_id_lo );

#inline
void OneWireHigh();
#inline
void OneWireLow();
void OneWireInit();

#inline
int OneWireRead();
int OneWirePresencePulse();
void OneWireWriteByte(int data);
int OneWireReadByte();


//tables
const int CRC8_TABLE[ 256 ] =
{
	0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
	157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
	35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
	190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
	70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
	219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
	101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
	248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
	140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
	17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
	175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
	50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
	202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
	87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
	233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
	116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};

const int CRC16_TABLE_LO[ 256 ] =
{
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41
	 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41
	 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40
};

const int CRC16_TABLE_HI[ 256 ] =
{
	 0x00, 0xc0, 0xc1, 0x01, 0xc3, 0x03, 0x02, 0xc2
	 0xc6, 0x06, 0x07, 0xc7, 0x05, 0xc5, 0xc4, 0x04
	 0xcc, 0x0c, 0x0d, 0xcd, 0x0f, 0xcf, 0xce, 0x0e
	 0x0a, 0xca, 0xcb, 0x0b, 0xc9, 0x09, 0x08, 0xc8
	 0xd8, 0x18, 0x19, 0xd9, 0x1b, 0xdb, 0xda, 0x1a
	 0x1e, 0xde, 0xdf, 0x1f, 0xdd, 0x1d, 0x1c, 0xdc
	 0x14, 0xd4, 0xd5, 0x15, 0xd7, 0x17, 0x16, 0xd6
	 0xd2, 0x12, 0x13, 0xd3, 0x11, 0xd1, 0xd0, 0x10
	 0xf0, 0x30, 0x31, 0xf1, 0x33, 0xf3, 0xf2, 0x32
	 0x36, 0xf6, 0xf7, 0x37, 0xf5, 0x35, 0x34, 0xf4
	 0x3c, 0xfc, 0xfd, 0x3d, 0xff, 0x3f, 0x3e, 0xfe
	 0xfa, 0x3a, 0x3b, 0xfb, 0x39, 0xf9, 0xf8, 0x38
	 0x28, 0xe8, 0xe9, 0x29, 0xeb, 0x2b, 0x2a, 0xea
	 0xee, 0x2e, 0x2f, 0xef, 0x2d, 0xed, 0xec, 0x2c
	 0xe4, 0x24, 0x25, 0xe5, 0x27, 0xe7, 0xe6, 0x26
	 0x22, 0xe2, 0xe3, 0x23, 0xe1, 0x21, 0x20, 0xe0
	 0xa0, 0x60, 0x61, 0xa1, 0x63, 0xa3, 0xa2, 0x62
	 0x66, 0xa6, 0xa7, 0x67, 0xa5, 0x65, 0x64, 0xa4
	 0x6c, 0xac, 0xad, 0x6d, 0xaf, 0x6f, 0x6e, 0xae
	 0xaa, 0x6a, 0x6b, 0xab, 0x69, 0xa9, 0xa8, 0x68
	 0x78, 0xb8, 0xb9, 0x79, 0xbb, 0x7b, 0x7a, 0xba
	 0xbe, 0x7e, 0x7f, 0xbf, 0x7d, 0xbd, 0xbc, 0x7c
	 0xb4, 0x74, 0x75, 0xb5, 0x77, 0xb7, 0xb6, 0x76
	 0x72, 0xb2, 0xb3, 0x73, 0xb1, 0x71, 0x70, 0xb0
	 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92
	 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54
	 0x9c, 0x5c, 0x5d, 0x9d, 0x5f, 0x9f, 0x9e, 0x5e
	 0x5a, 0x9a, 0x9b, 0x5b, 0x99, 0x59, 0x58, 0x98
	 0x88, 0x48, 0x49, 0x89, 0x4b, 0x8b, 0x8a, 0x4a
	 0x4e, 0x8e, 0x8f, 0x4f, 0x8d, 0x4d, 0x4c, 0x8c
	 0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86
	 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};


void IDChipInit()
{
	bit_set(TRIS_ID_CHIP_DATA);
	bit_clear(TRIS_ID_CHIP_POWER);
	IDChipOn();
}


void IDChipOn()
{
	output_high(PIN_ID_CHIP_POWER);
}


void IDChipOff()
{
	output_low(PIN_ID_CHIP_POWER);
}


// puts together id with first two bytes production_id from id-chip-eeprom, then byte 2-7 of unique id-chip-rom-number.
// production_id can be set with IDChipWriteProductionID.
#separate
int IDChipGetId(int *buff)
{
	unsigned int res;
	unsigned int i;
	unsigned int data_reserved[ 8 ];
	unsigned int* data;

		data = data_reserved;

		res=OneWirePresencePulse();

		OneWireWriteByte(ID_CHIP_COMM_READ_ROM);

		*data=OneWireReadByte();
		data++;
		*data=OneWireReadByte();
		data++;
		*data=OneWireReadByte();
		data++;
		*data=OneWireReadByte();
		data++;
		*data=OneWireReadByte();
		data++;
		*data=OneWireReadByte();
		data++;
		*data=OneWireReadByte();
		data++;
		*data=OneWireReadByte();
		data++;
		OneWirePresencePulse();

		data -= 8;

		if( res ) res = IDChipCheckCRCID( data );

		IDChipReadEeprom( 0, 2, buff );

		data += 6;
		buff += 2;

		if( res )
		{
			for( i = 0; i < 6; ++i )
			{
				*buff = *data;
				++buff;
				--data;
			}
		}
		else
		{
			for( i = 2; i < 8; ++i )
			{
				*buff = 0;
				++buff;
			}
		}

		*( buff - 8 ) = BOARD_ID_HIGH;
		*( buff - 7 ) = BOARD_ID_LOW;

		return res;
}


int IDChipCheckCRCID( unsigned int* data )
{
	unsigned int crc;
	unsigned int i;

		crc = 0;
		for( i = 0; i < 7; ++i )
		{
			crc = CRC8_TABLE[ crc ^ *data ];
			data++;
		}
		if( *data == crc ) return 1;
		else return 0;
}


// address must be among 0x to 127 for eeprom, 128 to 135 protection registers, 136 to 143 reserved bytes, should not be used.
// address must be first byte of a row of 8 bytes. that is 0, 8, 16, etc.
unsigned int IDChipReadEeprom( unsigned int address, unsigned int number_bytes, unsigned int* buff )
{
	unsigned int res;
	unsigned int i;

		if( address > 143 ) return 0;
		if(( address + number_bytes ) > 143 ) return 0;

		res = OneWirePresencePulse();

		OneWireWriteByte( ID_CHIP_COMM_SKIP_ROM );
		OneWireWriteByte( ID_CHIP_COMM_READ_MEMORY );
		OneWireWriteByte( address );
		OneWireWriteByte( ID_CHIP_ADDRESS_HI );

		for( i = 0; i < number_bytes; ++i )
		{
			*buff = OneWireReadByte();
			++buff;
		}

		OneWirePresencePulse();

		return res;
}


// writes block of 8bytes to an address.
// address must be among 0 to 127 for eeprom, 128 to 135 protection registers, 136 to 143 reserved bytes should not be used.
// address must be first byte of a row of 8 bytes. that is 0, 8, 16, etc.
unsigned int IDChipWriteEeprom( unsigned int address, unsigned int* buff )
{
	unsigned int res;
	unsigned int i;
	unsigned int crc[ 2 ];
	unsigned int dummy;

		if( address > 143 ) return 2;
		if(( address + 7 ) > 143 ) return 3;

		res = OneWirePresencePulse();

		OneWireWriteByte( ID_CHIP_COMM_SKIP_ROM );
		OneWireWriteByte( ID_CHIP_COMM_WRITE_SCRATCHPAD );
		OneWireWriteByte( address );
		OneWireWriteByte( ID_CHIP_ADDRESS_HI );
		OneWireWriteByte( *buff );
		++buff;
		OneWireWriteByte( *buff );
		++buff;
		OneWireWriteByte( *buff );
		++buff;
		OneWireWriteByte( *buff );
		++buff;
		OneWireWriteByte( *buff );
		++buff;
		OneWireWriteByte( *buff );
		++buff;
		OneWireWriteByte( *buff );
		++buff;
		OneWireWriteByte( *buff );
		++buff;
		crc[ 0 ] = OneWireReadByte();
		crc[ 1 ] = OneWireReadByte();
		OneWirePresencePulse();

		buff -= 8;

//	crc-check doesn't work at the moment......
//		if( IDChipCheckCRCScratchpad( crc, address, buff ))
//		{
			res = OneWirePresencePulse();

			OneWireWriteByte( ID_CHIP_COMM_SKIP_ROM );
			OneWireWriteByte( ID_CHIP_COMM_COPY_SCRATCHPAD );
			OneWireWriteByte( address );
			OneWireWriteByte( ID_CHIP_ADDRESS_HI );
			OneWireWriteByte( ID_CHIP_ES_BYTE );

			DelayMs( 13 );
			dummy = OneWireReadByte();
			if(( dummy == 170 ) || ( dummy == 85 )) res = 1;
			else res = 0;
//		}
//		else res = 0;

		return res;
}


// supposedly checks crc of a block of 8 bytes written to scratchpad, doesn't work at the moment!!
unsigned int IDChipCheckCRCScratchpad( unsigned int* crc, unsigned int address, unsigned int* buff )
{
	unsigned int crc_hi;
	unsigned int crc_lo;
	unsigned int dummy;
	unsigned int i;

		crc_lo = 0;
		crc_hi = 0;

		dummy = crc_lo ^ ID_CHIP_COMM_WRITE_SCRATCHPAD;
		crc_lo = CRC16_TABLE_LO[ dummy ] ^ crc_hi;
		crc_hi = CRC16_TABLE_HI[ dummy ];
//		test_crc[ 0 ] = crc_lo;
//		test_crc[ 1 ] = crc_hi;
		++buff;
		dummy = crc_lo ^ address;
		crc_lo = CRC16_TABLE_LO[ dummy ] ^ crc_hi;
		crc_hi = CRC16_TABLE_HI[ dummy ];
//		test_crc[ 2 ] = crc_lo;
//		test_crc[ 3 ] = crc_hi;
		++buff;
		dummy = crc_lo;
 		crc_lo = CRC16_TABLE_LO[ dummy ] ^ crc_hi;
		crc_hi = CRC16_TABLE_HI[ dummy ];
//		test_crc[ 4 ] = crc_lo;
//		test_crc[ 5 ] = crc_hi;
		++buff;

		for( i = 0; i < 8; ++i )
		{
			dummy = crc_lo ^ *buff;
			crc_lo = CRC16_TABLE_LO[ dummy ] ^ crc_hi;
			crc_hi = CRC16_TABLE_HI[ dummy ];
//			test_crc[ 6 + ( 2 * i )] = crc_lo;
//			test_crc[ 7 + ( 2 * i )] = crc_hi;
			++buff;
		}

		if(( crc_hi == crc[ 1 ] ) && ( crc_lo == crc[ 0 ] )) return 1;
		else return 0;
}


// writes a two byte production_id to address ID_CHIP_PRODUCTION_ID_ADDRESS.
unsigned int IDChipWriteProductionID( unsigned int production_id_hi, unsigned int production_id_lo )
{
	unsigned int buff[ 8 ];
	unsigned int* buff_p;
	unsigned int i;

		buff[ 0 ] = production_id_hi;
		buff[ 1 ] = production_id_lo;
		for( i = 2; i < 8; ++i ) buff[ i ] = 0;
		buff_p = buff;
		return IDChipWriteEeprom( ID_CHIP_PRODUCTION_ID_ADDRESS, buff_p );
}



//////////////////////////////////////////////////////////
// The DS2431 1-Wire routines.

#define PIN_ONE_WIRE	PIN_ID_CHIP_DATA
#define TRIS_ONE_WIRE	TRIS_ID_CHIP_DATA


#inline
void OneWireHigh()
{
   bit_set(TRIS_ONE_WIRE);
}


#inline
void OneWireLow()
{
   bit_clear(TRIS_ID_CHIP_DATA);//TRIS_ONE_WIRE);
   output_low(PIN_ID_CHIP_DATA);//PIN_ONE_WIRE);
}


#inline
int OneWireRead()
{
	if (input(PIN_ONE_WIRE)) return 1;
	else return 0;
}


// returns 1 if presence pulse answer was there
int OneWirePresencePulse()
{
	int buff;
	OneWireLow();
   	DelayUs(250);
  	DelayUs(250);

	OneWireHigh();
  	DelayUs(70);	//wait for the sample point

   	buff=OneWireRead();
   	DelayUs(250);
   	DelayUs(250);

   	if (buff==1) return 0; else return 1;
}


// least significant first
void OneWireWriteByte(int data)
{
	int i;
	for (i=0;i<=7;i++)
	{
		OneWireLow();
		if (bit_test(data,i))
		{
			DelayUs(10);
			OneWireHigh();
			DelayUs(75);
		}
		else
		{
			DelayUs(80);
			OneWireHigh();
			DelayUs(5);
		}
	}
}


// least significant first
int OneWireReadByte()
{
	int i,result;
	result=0;

	for (i=0;i<=7;i++)
	{
		OneWireLow();
		DelayUs(8);
		OneWireHigh();
		DelayUs(4);
		if (input(PIN_ONE_WIRE)) bit_set(result,i);
		DelayUs(65);	// finish the slot time
	}
	return result;
}


#endif
