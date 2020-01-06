/**
* driver for SPartEeprom
*
*
* Author: Albert Krohn  (krohn@teco.edu)
* Date: 2003-07-06 (yyyy-mm-dd)
*
* Adapted to SPart by sabin (wendhack@teco.edu)
* Date: 2004-01-21
*
* Current state: template
*/


//I2C Device!... needs the SParti2c driver

#define SPartEeprom_DRIVER	001




// EEprom adresses

#define EEPROM_EXT_ADDRESS_ID_HI    121		//start address for ID in external EEPROM:31112
#define EEPROM_EXT_ADDRESS_ID_LO	136		// from there on until 32767 is free (another 1655 byte)
#define EEPROM_EXT_ADDRESS_ID		31112	//same as long



/**
* Switches the SPartEeprom on
*/
void SPartEepromInit()
{
	SPartI2CInit();
}

void SPartEepromOn()
{
	SPartI2COn();
}

void SPartEepromOff()
{
	SPartI2COff();
}

int SPartEepromPrepare()
{
	SPartEepromOn();
}

//returns the state of the SPartEeprom: 0:not ready (e.g. writing), 1: ready
int SPartEepromReady()
{
	SPartI2CStart();
	if (!SPartI2CWrite(0xa0+XROM_ID)) {SPartI2CStop();return 1;}
	return 0;
}

//returns the number of bytes that fit into page, counted from addr
int SPartEepromBytesLeftInPage(long addr)
{
	long current_page;
	current_page=addr&0xFFC0;
	return (64-(int)(addr-current_page));
}

//returns 0 if successful
//1-byte modus, needs 5 ms wait time till next write process starts
int SPartEepromWriteByte(long addr,int data)
{
	SPartI2CStart();
	if (SPartI2CWrite(0xa0+XROM_ID)) {SPartI2CStop();return 1;}
	if (SPartI2CWrite(hi(addr))) {SPartI2CStop();return 2;}
	if (SPartI2CWrite(lo(addr)))  {SPartI2CStop();return 2;}
	if (SPartI2CWrite(data))  {SPartI2CStop();return 3;}
	SPartI2CStop();
	return 0;
}

//reads one single byte from addr, returns 0 if successful
int SPartEepromReadByte(long addr, int *dataout)
{
	SPartI2CStart();
	if (SPartI2CWrite(0xa0+XROM_ID)) {SPartI2CStop();return 1;}
	if (SPartI2CWrite(hi(addr))) {SPartI2CStop();return 2;}
	if (SPartI2CWrite(lo(addr)))  {SPartI2CStop();return 2;}
	SPartI2CStart();
	if (SPartI2CWrite(0xa1+XROM_ID)) {SPartI2CStop(); return 3;}
	*dataout=SPartI2CRead(0);
	SPartI2CStop();

	return 0;
}


//returns 0 if successful
//sequential write modus, will only write,if data fits into current page.
int SPartEepromWriteSequence(long addr, int* buff, long count)
{
	long i;
	i=0;
	if(SPartEepromBytesLeftInPage(addr)<count) return 4;		//data does not fit into page
	SPartI2CStart();
	if (SPartI2CWrite(0xa0+XROM_ID)) {SPartI2CStop();return 1;}
	if (SPartI2CWrite(hi(addr))) {SPartI2CStop();return 2;}
	if (SPartI2CWrite(lo(addr)))  {SPartI2CStop();return 2;}
	while(i<count)
	{
		if (SPartI2CWrite(buff[i]))  {SPartI2CStop();return 3;}
		i++;
	}
	SPartI2CStop();
	return 0;
}


//returns 0 if successful
int SPartEepromReadSequence(long addr,int* buff,long count)
{
	long i;
	i=0;
	SPartI2CStart();
	if (SPartI2CWrite(0xa0+XROM_ID)) {SPartI2CStop(); return 1;}
	if (SPartI2CWrite(hi(addr))) {SPartI2CStop(); return 2;}
	if (SPartI2CWrite(lo(addr))) {SPartI2CStop(); return 2;}
	SPartI2CStart();
	if (SPartI2CWrite(0xa1+XROM_ID)) {SPartI2CStop(); return 3;}
	for(i=0;i<count-1;i++)	buff[i]=SPartI2CRead(1);
	buff[count-1]=SPartI2CRead(0);
	SPartI2CStop();
	return 0;
}


//return 0 if succesful, returns 1 if value is false, returns 2,3 or 4 if
//address isn't acknowledged
int SPartEepromTest()
{
	unsigned int data;

	SPartI2CStart();
	if (SPartI2CWrite(0xa0+XROM_ID)) {SPartI2CStop();return 1;}
	if (SPartI2CWrite(0x00)) {SPartI2CStop();return 2;}
	if (SPartI2CWrite(0x10)) {SPartI2CStop();return 3;}
	if (SPartI2CWrite(44))  {SPartI2CStop();return 4;}
	SPartI2CStop();

	delay_ms(5);	//wait during SPartEeprom write cycle

	SPartI2CStart();
	if (SPartI2CWrite(0xa0+XROM_ID)) {SPartI2CStop();return 5;}
	if (SPartI2CWrite(0x00)) {SPartI2CStop();return 6;}
	if (SPartI2CWrite(0x10))  {SPartI2CStop();return 7;}
	SPartI2CStart();
	if (SPartI2CWrite(0xa1+XROM_ID)) {SPartI2CStop(); return 8;}
	data = SPartI2CRead(0);
	SPartI2CStop();

	if (data==44) return 0;
	else return 9;
}


