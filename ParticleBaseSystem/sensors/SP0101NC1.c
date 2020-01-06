/**
* Sensor driver for microphone SP0101NC1
*
* Provides low and high level functions for initializing and sampling of the
* microphone
*
* Author: Albert Krohn  (krohn@teco.edu)
* Date: 2003-06-11 (yyyy-mm-dd)
*
* version 001 (2003-12-03, cdecker)
*   - driver on new software architecure
*
* version 002 (2003-12-04, cdecker)
*   - error in volume computation which causes infinte loop corrected
*/


#define AUDIO_SENSOR			002


/**
* Switches the micro amp on
*/
#define AudioSensorOn()	output_high(PIN_AUDIO_MIC_POWER)

/**
* Switches the micro amp off
*/
#define AudioSensorOff()	output_low(PIN_AUDIO_MIC_POWER)


/**
* Initializes the Audio microphone amp
* Sets power to output and sampling pin to input.
*
* @return returns 0 if no error
*/
#ifdef PIN_AUDIO_MIC_OUT
int AudioSensorInit()
{
	bit_clear(TRIS_AUDIO_MIC_POWER);		// set power pin to output
	bit_set(TRIS_AUDIO_MIC_OUT);			// set sample pin to input

	AudioSensorOff();						// power off for light
	return 0;								// indicate no error
}
#endif



/**
* Gets the sensor value
* @param value the sensor value
* @return returns 0 if no error
*/
int AudioSensorGet(int* value)
{

	*value = ADCRead8();
	return 0;
}



/**
* Setups the microcontroller for sampling
* Sets the correct AD channel, if neccessary
* @return returns 0 if no error
*/
int AudioSensorPrepare()
{
	// set channel
	if ( !ADCIsChSet(ADCH_AUDIO_MIC) ) {
		ADCSetCh(ADCH_AUDIO_MIC);
	}

	// set conversion clock
	ADCSetClock(0);

	// start ADC
	ADCEnable();

	return 0;
}



/**
* Computes the average power of the audio signal given an
* array of sampled values
* @param AudioRawData array of sampled audio values
* @param AudioArraySize array size
* @return the volume
*/
int AudioCalcVolume(int* AudioRawData, int AudioArraySize)
{
	int i;
	int DCoffset;
	int Pave;
	long TempSum;
	int temp;



	//first calc DCoffset as median = E{AudioRawData}

	TempSum=0;
	for (i=0;i<AudioArraySize;i++)
	{
		TempSum=TempSum+ (long) AudioRawData[i];
	}
	DCoffset=(int) (TempSum/AudioArraySize);


	//now calc average Power of Signal

	Pave=0;
	temp=0;
	TempSum=0;
	for (i=0;i<AudioArraySize;i++)
	{
		if (AudioRawData[i] <= DCoffset)
			{
				temp= DCoffset - AudioRawData[i];
			}
		else
			{
				temp= AudioRawData[i]-DCoffset;
			}

		TempSum=TempSum+(long) temp	;
	}

	Pave=(int) (TempSum/AudioArraySize);


	return (Pave);
}



/**
* Computes the average power of the audio signal.
* This power is computed
* @param volume pointer for storing the volume level
* @return returns 0 if no error
*/
int AudioSensorGetVolume(unsigned int* volume)
{
	unsigned long TempSum;
	unsigned long TempSum2;

	int i;
	int DCoffset;
	int Pave;
	int temp;


	//first calc DCoffset as median = E{AudioRawData}
	// 2 loops acquire 512 samples, this method avoids data types longer than a long

	TempSum=0;
	for (i=0;i<128;i++)
	{
		AudioSensorGet(&temp);
		TempSum=TempSum+temp;
	}

/*
	TempSum2=0;
	for (i=0;i<256;i++)
	{
		AudioSensorGet(&temp);
		TempSum2=TempSum2+temp;
	}


	DCoffset=(int) ( (TempSum>>9) + (TempSum2>>9) );
*/
	DCoffset=(int) ( (TempSum>>7) );



	//now calc average Power of Signal
	// same procedure as for DCoffset

	TempSum=0;
	TempSum2=0;

	for (i=0;i<128;i++)
	{
		AudioSensorGet(&temp);
		if (temp <= DCoffset)
		{
			temp= DCoffset - temp;
		}
		else
		{
			temp= temp -DCoffset;
		}
		TempSum=TempSum+temp;
	}

/*
	for (i=0;i<256;i++)
	{
		AudioSensorGet(&temp);
		if (temp <= DCoffset)
		{
			temp= DCoffset - temp;
		}
		else
		{
			temp= temp -DCoffset;
		}
		TempSum=TempSum+temp;
	}

	Pave= (int) ( (TempSum>>9) + (TempSum2>>9) );
*/
	Pave= (int) ( (TempSum>>7) );

	*volume=Pave;
	return 0;

}

/**
* Packs the audio sensor value directly in the sendbuffer
* as an ACL packet which is ready to be sent.
* @param type type of audio sensor value (e.g. 1 for mean volume)
* @param data pointer to data array of audio value(s)
* @param dataLength length of data array
* @return error code, 0 if no error
*
* Note: The format is described in acltypes.h
*/
int AudioSensorPackInACL(int type, byte* data, unsigned int dataLength)
{
	int i;

	if ( ACLGetRemainingPayloadSpace() > 5 + dataLength)
	{
		ACLAddNewType(ACL_TYPE_SAU_HI,ACL_TYPE_SAU_LO); // SAU
		ACLAddData(type);								// value
		for (i = 0; i < dataLength; i++)
		{
			ACLAddData(data[i]);						// value(s)
		}
		ACLAddData(0);									// index = 0
	}
	else
	{
		return 1; 	// not enough space left
	}

	return 0; // no error
}
