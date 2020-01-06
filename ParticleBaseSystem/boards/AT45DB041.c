/*
*
*	particle flash driver for the atmel AT45DB041B working on the 220 pc
*	todo: define selfttest loopback test pins
*/


/**
* file version: 001 (2004-0707 krohn)
*
*/


#ifndef __FLASH_C__
#define __FLASH_C__             001





/*

	data is clocked into flash with rising edge
	data is clocked out with falling edge

*/


#define FLASH_DONT_CARE							0x00

//OPCODES
#define FLASH_BUFFER_1_WRITE					0x84
#define FLASH_BUFFER_2_WRITE					0x87
#define FLASH_BUFFER_1_READ   					0x54
#define FLASH_BUFFER_2_READ						0x56

#define FLASH_STATUS_READ						0x57
#define	FLASH_FLASH_CONTINUOUS_READ				0x68
#define FLASH_PAGE_READ								0x52		// doesn't stop automatically, starts again from beginning of page...
#define FLASH_BUFFER_1_TO_PAGE_ERASE_PROGRAM	0x83

#define FLASH_BUFFER_2_TO_PAGE_ERASE_PROGRAM	0x86
#define FLASH_BUFFER_1_TO_PAGE_PROGRAM			0x88		// has to be erased before.
#define FLASH_BUFFER_2_TO_PAGE_PROGRAM			0x89		// has to be erased before.
#define FLASH_PAGE_ERASE						0x81
#define FLASH_BLOCK_ERASE						0x50		// erases 8 pages, addresses from 0 to 255.

#define FLASH_PAGE_PROGRAM_THRU_BUFFER_1		0x82
#define FLASH_PAGE_PROGRAM_THRU_BUFFER_2		0x85

#define FLASH_PAGE_TO_BUFFER_1_TRANSFER			0x53
#define FLASH_PAGE_TO_BUFFER_2_TRANSFER			0x55
#define FLASH_PAGE_TO_BUFFER_1_COMPARE			0x53
#define FLASH_PAGE_TO_BUFFER_2_COMPARE			0x55//??? opcode
#define FLASH_PAGE_REWRITE_THRU_BUFFER_1		0x58
#define FLASH_PAGE_REWRITE_THRU_BUFFER_2		0x59



// some addresses
#define FLASH_ID_ADDRESS 540408		//start address for ID in external flash: last page, first byte






int32 flash_old_write_address;
boolean	flash_write_done;
int flash_active_write_buffer;




void FlashEnable()
{
	SPIInit(SPI_MODE_FAST);
	output_low(PIN_FLASH_CS);
}

void FlashDisable()
{
	output_high(PIN_FLASH_CS);
}

void FlashOff()
{
	output_low(PIN_FLASH_POWER);		// turn on
}

void FlashOn()
{
	output_high(PIN_FLASH_POWER);		// turn on
}



void FlashInit()
{
	//set the states correctly
	//set all pins correctly


	bit_clear(TRIS_FLASH_POWER);
	FlashOn();

	bit_clear(TRIS_FLASH_CS);
	FlashDisable();

	DelayMs(100);	//wait time according spec after power on


	flash_old_write_address=0xFFFFFFFF;		//set the old address to a value out of range to force a page load at first access
 	flash_active_write_buffer=1;
	flash_write_done=true;
}


int FlashReadSequenceFromBuffer1(long pos,byte*data, long len)
{
	byte buff;
	long i;

	FlashEnable();

	SPIShift(FLASH_BUFFER_1_READ);
	SPIShift(FLASH_DONT_CARE);
	SPIShift(hi(pos));
	SPIShift(lo(pos));
	SPIShift(FLASH_DONT_CARE);

	for (i=0;i<len;i++)
	{
		*data=SPIShift(FLASH_DONT_CARE);//one read instruction
		data++;
	}


	FlashDisable();
	return buff;
}

int FlashReadSequenceFromBuffer2(long pos,byte*data, long len)
{
	byte buff;
	long i;

	FlashEnable();

	SPIShift(FLASH_BUFFER_2_READ);
	SPIShift(FLASH_DONT_CARE);
	SPIShift(hi(pos));
	SPIShift(lo(pos));
	SPIShift(FLASH_DONT_CARE);

	for (i=0;i<len;i++)
	{
		*data=SPIShift(FLASH_DONT_CARE);//one read instruction
		data++;
	}


	FlashDisable();
	return buff;
}
byte FlashReadFromBuffer1(long pos)
{
	byte buff;
	long i;

	FlashEnable();

	SPIShift(FLASH_BUFFER_1_READ);
	SPIShift(FLASH_DONT_CARE);
	SPIShift(hi(pos));
	SPIShift(lo(pos));
	SPIShift(FLASH_DONT_CARE);

	buff	=SPIShift(FLASH_DONT_CARE);//one read instruction


	FlashDisable();
	return buff;
}

// nine bit are valid from addr, remains is dont care
byte FlashWriteToBuffer1(long addr, byte b)
{
	byte buff;

	FlashEnable();

	SPIShift(FLASH_BUFFER_1_WRITE);
	SPIShift(0);
	SPIShift(hi(addr));
	SPIShift(lo(addr));

	buff=SPIShift(b);

	FlashDisable();
	return buff;
}

// nine bit are valid from addr, remains is dont care
byte FlashWriteToBuffer2(long addr, byte b)
{
	byte buff;

	FlashEnable();

	SPIShift(FLASH_BUFFER_2_WRITE);
	SPIShift(0);
	SPIShift(hi(addr));
	SPIShift(lo(addr));

	buff=SPIShift(b);

	FlashDisable();
	return buff;
}





boolean FlashReady()
{
	byte status;

	FlashEnable();
	SPIShift( FLASH_STATUS_READ );
	status = SPIShift( FLASH_DONT_CARE );
	FlashDisable();
	if( bit_test( status, 7 )) return true; else return false;
}


void FlashWaitForReady()
{
	while (!FlashReady()) ;  //DelayMs(1);	//let the flash time to do its stuff, wait a ms
}

//returns true if two adresses are in the same page
boolean FlashInSamePageOld(int32 addr1, int32 addr2)
{

	if (  ((long)(addr1/264)) == ((long)(addr2/264))  )
	{

		return true;
	}
	else
	{

		return false;
	}

}

boolean FlashInSamePage(int32 addr1, int32 addr2)
{
	if (addr2>addr1)
	{
		if ( ((addr1%264)+(addr2-addr1))>=264) return false;
	}
	else
	{
		if ( ((addr2%264)+(addr1-addr2)) >=264) return false;
	}
	return true;
}


int FlashSendAddress(int32 flash_address)
{
	long pagenr;
	long bytenr;


	pagenr=(long) (flash_address/264);
	bytenr=(long) (flash_address-( (int32)(pagenr)*264 )     );

	pagenr<<=1;
	if (bit_test(bytenr,8)) bit_set(pagenr,0);

	//res[0]=hi(pagenr);
	//res[1]=lo(pagenr);
	//res[2]=hi(bytenr);
	//res[2]=lo(bytenr);


	SPIShift(hi(pagenr));
	SPIShift(lo(pagenr));
	SPIShift(lo(bytenr));


/*
		SPIShift(0);
		SPIShift(0);
		SPIShift(0);
*/
}

long FlashGetBytePos_old(int32 flash_addr)
{
	long pagenr;
	long bytenr;

	pagenr=(long) (flash_addr/264);
	bytenr=(long) (flash_addr-( (int32)(pagenr)*264 )     );
	return bytenr;
}

long FlashGetBytePos(int32 flash_addr)
{
	return (long) (flash_addr % 264);
}


// this function positions the read pointer to a certain position and then the user can get continuous data by using SPIShift(),
// no flash disable is processed!!!
int FlashSetReadPointer(int32 flash_start_address)
{
		if (!FlashReady()) return 0;
		FlashEnable();
		SPIShift(FLASH_FLASH_CONTINUOUS_READ);
		FlashSendAddress(flash_start_address );
		SPIShift(FLASH_DONT_CARE);
		SPIShift(FLASH_DONT_CARE);
		SPIShift(FLASH_DONT_CARE);
		SPIShift(FLASH_DONT_CARE);
}



// this funciton store the RAM into the page (and erases before) where flash_address belongs to
int FlashStoreBuffer1(int32 flash_address)
{
	FlashEnable();
	//if (!FlashReady()) return 0;
	SPIShift(FLASH_BUFFER_1_TO_PAGE_ERASE_PROGRAM);

	FlashSendAddress(flash_address);
	FlashDisable();
}

// this funciton store the RAM into the page (and erases before) where flash_address belongs to
int FlashStoreBuffer2(int32 flash_address)
{
	FlashEnable();
	//if (!FlashReady()) return 0;
	SPIShift(FLASH_BUFFER_2_TO_PAGE_ERASE_PROGRAM);
	FlashSendAddress(flash_address);
	FlashDisable();
}

// this funciton loads a page (flash_address belongs to) into the buffer1 RAM
int FlashLoadToBuffer1(int32 flash_address)
{
	FlashEnable();
	//if (!FlashReady()) return 0;
	SPIShift(FLASH_PAGE_TO_BUFFER_1_TRANSFER);
	FlashSendAddress(flash_address);
	FlashDisable();
}

// this funciton loads a page (flash_address belongs to) into the buffer1 RAM
int FlashLoadToBuffer2(int32 flash_address)
{
	FlashEnable();
	//if (!FlashReady()) return 0;
	SPIShift(FLASH_PAGE_TO_BUFFER_2_TRANSFER);
	FlashSendAddress(flash_address);
	FlashDisable();
}


//stores the active RAM buff in a page if necessary
int FlashFlush()
{

	FlashWaitForReady();



	if (flash_write_done==false)
	{

		if (flash_active_write_buffer==1)
		{
			FlashStoreBuffer1(flash_old_write_address);
			flash_write_done=true;
		}
		else
		{
			FlashStoreBuffer2(flash_old_write_address);
			flash_write_done=true;
		}
	}
	FlashWaitForReady();
}




int FlashChangeWritePage(int32 old_addr,int32 new_addr)
{
	//the active page has changed and the content of RAM has not yet been saved
	// so first load/save the buffers
	if (!FlashReady()) return 0;
	{
		if (flash_active_write_buffer==1)
		{
			//load new page to buffer2
			FlashLoadToBuffer2(new_addr);
			FlashWaitForReady();
			if (flash_write_done==false)
			{
				FlashStoreBuffer1(old_addr);
				flash_write_done=true;
			}
			flash_active_write_buffer=2;
			flash_old_write_address=new_addr;
		}
		else
		{
			//load new page to buffer1
			FlashLoadToBuffer1(new_addr);
			FlashWaitForReady();
			if (flash_write_done==false)
			{
				FlashStoreBuffer2(old_addr);
				flash_write_done=true;
			}

			flash_active_write_buffer=1;
			flash_old_write_address=new_addr;
		}
	}
}

//this function uses RAM buffer 1
// return = 0 for bad, 1 for good
// if cannot handle 2 page switches!!! (buffer longer 264)
int FlashWriteSequence(int32 flash_start_address, byte* data, long count)
{
	long i,buffer_pos;
	int32 write_address;

	if (count>264) return 0;


	if (!FlashInSamePage(flash_start_address,flash_old_write_address))
	{

		if (!FlashReady()) return 0;
		FlashChangeWritePage(flash_old_write_address,flash_start_address);//the active page has changed and the content of RAM has not yet been saved so first load/save the buffers
	}


	// now check if a page switch will be neccessary during writing, if so, abort if the last write is still in progess
	if ((!(FlashInSamePage(flash_start_address,flash_start_address+(int32)count))) && (!FlashReady())) return 0;

	buffer_pos=FlashGetBytePos(flash_start_address);
	write_address=flash_start_address;

	FlashEnable();
	if (flash_active_write_buffer==1) SPIShift(FLASH_BUFFER_1_WRITE); else SPIShift(FLASH_BUFFER_2_WRITE);
	FlashSendAddress(buffer_pos);

	for (i=0;i<count;i++)
	{
		if (buffer_pos==264)
		{

			buffer_pos=0;
			FlashDisable();
			if (!FlashReady()) return 0;
			//do a page switch
			FlashChangeWritePage(write_address-1,write_address);
			//buffer_pos=buffer_pos-264;
			FlashEnable();
			buffer_pos=0;
			if (flash_active_write_buffer==1) SPIShift(FLASH_BUFFER_1_WRITE); else SPIShift(FLASH_BUFFER_2_WRITE);
			FlashSendAddress(buffer_pos);

		}

		SPIShift(*data);
		flash_write_done=false;
		flash_old_write_address=write_address;
		buffer_pos++;
		write_address++;
		data++;


	}
	FlashDisable();

	return 1;


}



int FlashReadContinuous(int32 flash_start_address, byte* data, long count)
{

	long pos;

	if (!FlashReady()) return 0;
	FlashEnable();
	SPIShift(FLASH_FLASH_CONTINUOUS_READ);
	FlashSendAddress(flash_start_address );
	SPIShift(FLASH_DONT_CARE);
	SPIShift(FLASH_DONT_CARE);
	SPIShift(FLASH_DONT_CARE);
	SPIShift(FLASH_DONT_CARE);

	for (pos=0;pos<count;pos++)
	{
		*data=SPIShift(FLASH_DONT_CARE);
		data++;
	}
	FlashDisable();
	return 1;

}

int FlashReadSequence(int32 flash_start_address, byte* data, long count)
{
	long pos,startPos,saveCount,flashReadBeforeRAMCount;

	//
	// A write before a read may create a true dependency and therefore result
	// in reading on old data from the flash because the recent data weren't
	// written back to the flash.
	//
	// Solution 1:
	// Explictit write back by calling FlashFlush
	// This costs tiiiiiiiiiiiime!!!!!!!
	//
	// Solution 2:
	// Read from RAM buffer the last recent data
	// Fast, but more complex.
	//
	// Here: we go for solution 2.
	//
	// ToDo: write this function better: FLASH/RAM/FLASH pattern


	if (!FlashReady()) return 0;

	if (  FlashInSamePage(flash_start_address,flash_old_write_address) )
	{

		//
		// first data portion is already in RAM buffer,
		// afterwards read continously from flash
		//

		pos = FlashGetBytePos(flash_start_address); // start address in RAM buffer
		saveCount = count;

		// read RAM buffer, there is the last recent version of page
		FlashEnable();
		if ( flash_active_write_buffer == 1 )
		{
			SPIShift(FLASH_BUFFER_1_READ);
			FlashSendAddress(flash_start_address);
			SPIShift(FLASH_DONT_CARE);
		}

		if ( flash_active_write_buffer == 2 )
		{
			SPIShift(FLASH_BUFFER_2_READ);
			FlashSendAddress(flash_start_address);
			SPIShift(FLASH_DONT_CARE);
		}


		while ( count )
		{

			if (pos == 264) break; // end of page, but no end of read

			*data = SPIShift(FLASH_DONT_CARE); // from RAM buffer
			data++;
			pos++;
			count--;
		}
		FlashDisable();

		if (count)
		{
			// from now read continuously
			FlashReadContinuous(flash_start_address + (saveCount - count), data, count);
		}

		return 1;  // done, return with ok

	}

	// check whether we encounter the buffer
	if ( (flash_start_address <= flash_old_write_address) && (flash_start_address + count) >= flash_old_write_address )
	{
		// during read, we will reach one of the buffers containing
		// the last recent portion of data

		// determine address borders for reading FLASH/RAM/FLASH
		saveCount = count;
		flashReadBeforeRAMCount = flash_old_write_address - flash_start_address - FlashGetBytePos(flash_old_write_address);
		pos = 0;

		//
		// read FLASH
		//
//		if ( flashReadCountBeforeRAM )
//		{
			FlashReadContinuous( flash_start_address, data, flashReadBeforeRAMCount );
			count -= flashReadBeforeRAMCount;
			data += flashReadBeforeRAMCount;
//		}

		//
		// read RAM
		//
		FlashEnable();
		if ( flash_active_write_buffer == 1 )
		{
			SPIShift(FLASH_BUFFER_1_READ);
			FlashSendAddress(flash_start_address+flashReadBeforeRAMCount);
			SPIShift(FLASH_DONT_CARE);
		}

		if ( flash_active_write_buffer == 2 )
		{
			SPIShift(FLASH_BUFFER_2_READ);
			FlashSendAddress(flash_start_address+flashReadBeforeRAMCount);
			SPIShift(FLASH_DONT_CARE);
		}

		while ( count )
		{

			if (pos == 264) break; // end of page, but no end of read

			*data = SPIShift(FLASH_DONT_CARE); // from RAM buffer
			data++;
			pos++;
			count--;
		}
		FlashDisable();

		//
		// read FLASH
		//
		if (count)
		{
			// from now read continously
			FlashReadContinuous( flash_start_address + (saveCount-count), data, count);
		}

		return 1;  // done, return with ok
	}

	//
	// we do not encounter a buffer, so read continously from flash
	//
	FlashReadContinuous(flash_start_address, data, count);

	return 1;  // done, return with ok
}



#endif