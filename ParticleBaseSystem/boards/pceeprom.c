/**
* driver for PCEeprom
*
*
* Author: Albert Krohn  (krohn@teco.edu)
* Date: 2003-07-06 (yyyy-mm-dd)
*
* Current state: template
*/


//I2C Device!... needs the PCi2c driver

#define PCEeprom_DRIVER	001




// EEprom adresses

#define EEPROM_EXT_ADDRESS_ID_HI    121		//start address for ID in external EEPROM:31112
#define EEPROM_EXT_ADDRESS_ID_LO	136		// from there on until 32767 is free (another 1655 byte)
#define EEPROM_EXT_ADDRESS_ID		31112	//same as long



/**
* Switches the PCEeprom on
*/
void PCEepromInit()
{
	PCI2CInit();
}

void PCEepromOn()
{
	PCI2COn();
}

void PCEepromOff()
{
	PCI2COff();
}

int PCEepromPrepare()
{
	PCEepromOn();
}

//returns the state of the PCEeprom: 0:not ready (e.g. writing), 1: ready
int PCEepromReady()
{

	PCI2CStart();
	if (!PCI2CWrite(0xa0+XROM_ID)) {PCI2CStop();return 1;}
	return 0;
}

//returns the number of bytes that fit into page, counted from addr
int PCEepromBytesLeftInPage(long addr)
{
	long current_page;
	current_page=addr&0xFFC0;
	return (64-(int)(addr-current_page));
}

//returns 0 if successful
//1-byte modus, needs 5 ms wait time till next write process starts
int PCEepromWriteByte(long addr,int data)
{
	PCI2CStart();
	if (PCI2CWrite(0xa0+XROM_ID)) {PCI2CStop();return 1;}
	if (PCI2CWrite(hi(addr))) {PCI2CStop();return 2;}
	if (PCI2CWrite(lo(addr)))  {PCI2CStop();return 2;}
	if (PCI2CWrite(data))  {PCI2CStop();return 3;}
	PCI2CStop();
	return 0;
}

//reads one single byte from addr, returns 0 if successful
int PCEepromReadByte(long addr, int *dataout)
{
	PCI2CStart();
	if (PCI2CWrite(0xa0+XROM_ID)) {PCI2CStop();return 1;}
	if (PCI2CWrite(hi(addr))) {PCI2CStop();return 2;}
	if (PCI2CWrite(lo(addr)))  {PCI2CStop();return 2;}
	PCI2CStart();
	if (PCI2CWrite(0xa1+XROM_ID)) {PCI2CStop(); return 3;}
	*dataout=PCI2CRead(0);
	PCI2CStop();

	return 0;
}


//returns 0 if successful
//sequential write modus, will only write,if data fits into current page.
int PCEepromWriteSequence(long addr, int* buff, long count)
{
	long i;
	i=0;
	if(PCEepromBytesLeftInPage(addr)<count) return 4;		//data does not fit into page
	PCI2CStart();
	if (PCI2CWrite(0xa0+XROM_ID)) {PCI2CStop();return 1;}
	if (PCI2CWrite(hi(addr))) {PCI2CStop();return 2;}
	if (PCI2CWrite(lo(addr)))  {PCI2CStop();return 2;}
	while(i<count)
	{
		if (PCI2CWrite(buff[i]))  {PCI2CStop();return 3;}
		i++;
	}
	PCI2CStop();
	return 0;
}


//returns 0 if successful
int PCEepromReadSequence(long addr,int* buff,long count)
{
	long i;
	i=0;
	PCI2CStart();
	if (PCI2CWrite(0xa0+XROM_ID)) {PCI2CStop(); return 1;}
	if (PCI2CWrite(hi(addr))) {PCI2CStop(); return 2;}
	if (PCI2CWrite(lo(addr))) {PCI2CStop(); return 2;}
	PCI2CStart();
	if (PCI2CWrite(0xa1+XROM_ID)) {PCI2CStop(); return 3;}
	for(i=0;i<count-1;i++)	buff[i]=PCI2CRead(1);
	buff[count-1]=PCI2CRead(0);
	PCI2CStop();
	return 0;
}


//return 0 if succesful, returns 1 if value is false, returns 2,3 or 4 if
//address isn't acknowledged
int PCEepromTest()
{
	int data;

	PCI2CStart();
	if (PCI2CWrite(0xa0+XROM_ID))
		{PCI2CStop(); return 2;}
	PCI2CWrite(0x79);	//address is 31110
	PCI2CWrite(0x86);

	PCI2CWrite(44);
	PCI2CStop();
	DelayMs(5);	//wait during PCEeprom write cycle

	PCI2CStart();
	if (PCI2CWrite(0xa0+XROM_ID))
		{PCI2CStop(); return 3;}
	PCI2CWrite(0x79);
	PCI2CWrite(0x86);
	PCI2CStart();
	if (PCI2CWrite(0xa1+XROM_ID))
		{PCI2CStop(); return 4;}
	data=PCI2CRead(0);
	PCI2CStop();
	if (data==44) return 0;
	else return 1;
}


