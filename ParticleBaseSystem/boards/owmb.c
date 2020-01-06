/**
* driver for magnetic buzzer owmb 505025
* MAKE SURE THAT NO INTERRUPTS OCCURR DURING BUZZERS!!!!
*
*
* Author: Albert Krohn krohn@teco.edu
* Date: 2004-07-30 (yyyy-mm-dd)
*
* Current state: ok
*
* version 001: 	2004-11-10 by sabin (wendhack@teco.edu)
*
*
*/


#define BUZZER_MODULE	001

#define TONE_C3		130
#define TONE_D3		146
#define TONE_E3		164
#define TONE_F3		174
#define TONE_G3		196
#define TONE_A3		220
#define TONE_H3		246

#define TONE_C4		261
#define TONE_D4		293
#define TONE_E4		329
#define TONE_F4		349
#define TONE_G4		392
#define TONE_A4		440
#define TONE_H4		493

#define TONE_C5		523
#define TONE_D5		587
#define TONE_E5		659
#define TONE_F5		698
#define TONE_G5		783
#define TONE_A5		880
#define TONE_H5		987

#define TONE_C6		1046
#define TONE_D6		1174
#define TONE_E6		1318
#define TONE_F6		1396
#define TONE_G6		1567
#define TONE_A6		1760
#define TONE_H6		1975

#define TONE_C7		2093
#define TONE_D7		2349
#define TONE_E7		2637
#define TONE_F7		2793
#define TONE_G7		3135
#define TONE_A7		3520
#define TONE_H7		3951

#define TONE_C8		4186
#define TONE_D8		4700
#define TONE_E8		5280
#define TONE_F8		5586
#define TONE_G8		6270
#define TONE_A8		7040
#define TONE_H8		7900

#define TONE_C9		8370

/**
* Switches the micro amp off
*/




int sound_volume;

void BuzzerOff()
{
	output_low(PIN_BUZZER);
}


void BuzzerSetVolume(int vol)
{
	if (vol>20) vol=20;
	sound_volume=vol;
}

int BuzzerGetVolume()
{
	return sound_volume;
}


/**
* Initializes the Buzzer
*/
int BuzzerInit()
{
	bit_clear(TRIS_BUZZER);				// set sound pin to output
	BuzzerOff();
	sound_volume=20;						//max
	return 0;								// indicate no error
}


//frequency in hz, duration in ms, duty in us (the high time)( for high=0=50%duty)
void BuzzerTone(long frequency,long duration)
{
	long i,T,cycles;
	long T1,T2;
	float f,d,tt;

	f=(float) frequency;
	d=(float) duration;
	tt=(1000000/f);
	T=(long)tt;
	T=T-(long)sound_volume;

	cycles=(long)( ((float)duration)*f/1000);

	for(i=0;i<cycles;i++)
	{
		output_high(PIN_BUZZER);
		DelayUs(sound_volume);
		output_low(PIN_BUZZER);
		DelayUsLong(T);
	}
	BuzzerOff();		//just to make sure
}


void BuzzerSweepUp()
{
	long T;

	for (T=2000;T>166;T=T-5)
	{
		output_high(PIN_BUZZER);
		DelayUs(10);
		output_low(PIN_BUZZER);
		DelayUsLong(T);
	}
}


void BuzzerSweepDown()
{
	long T;

	for (T=166;T<2000;T=T+5)
	{
		output_high(PIN_BUZZER);
		DelayUs(10);
		output_low(PIN_BUZZER);
		DelayUsLong(T);
	}
}


void BuzzerPlayASong(int song)
{
	switch(song)

	{
		case 1:	// particle startup
				BuzzerTone(TONE_G7,70);
				BuzzerTone(TONE_C7,80);
				DelayMs(20);
				BuzzerTone(TONE_C7,100);
				BuzzerTone(TONE_D7,200);
				DelayMs(100);
				BuzzerTone(TONE_D7,400);
				break;




		case 2:
				//alle meine Entchen, tief
				BuzzerTone(TONE_C6,100);
				BuzzerTone(TONE_D6,100);
				BuzzerTone(TONE_E6,100);
				BuzzerTone(TONE_F6,100);
				BuzzerTone(TONE_G6,150);DelayMs(50);
				BuzzerTone(TONE_G6,150);DelayMs(50);
				BuzzerTone(TONE_A6,50);DelayMs(50);
				BuzzerTone(TONE_A6,50);DelayMs(50);
				BuzzerTone(TONE_A6,50);DelayMs(50);
				BuzzerTone(TONE_A6,50);DelayMs(50);
				BuzzerTone(TONE_G6,400);
				BuzzerTone(TONE_A6,50);DelayMs(50);
				BuzzerTone(TONE_A6,50);DelayMs(50);
				BuzzerTone(TONE_A6,50);DelayMs(50);
				BuzzerTone(TONE_A6,50);DelayMs(50);
				BuzzerTone(TONE_G6,400);
				BuzzerTone(TONE_F6,50);DelayMs(50);
				BuzzerTone(TONE_F6,50);DelayMs(50);
				BuzzerTone(TONE_F6,50);DelayMs(50);
				BuzzerTone(TONE_F6,50);DelayMs(50);
				BuzzerTone(TONE_E6,150);DelayMs(50);
				BuzzerTone(TONE_E6,150);DelayMs(50);
				BuzzerTone(TONE_D6,50);DelayMs(50);
				BuzzerTone(TONE_D6,50);DelayMs(50);
				BuzzerTone(TONE_D6,50);DelayMs(50);
				BuzzerTone(TONE_D6,50);DelayMs(50);
				BuzzerTone(TONE_C6,400);
				break;
		case 3:
				//alle meine Entchen super hoch
				BuzzerTone(TONE_C8,100);
				BuzzerTone(TONE_D8,100);
				BuzzerTone(TONE_E8,100);
				BuzzerTone(TONE_F8,100);
				BuzzerTone(TONE_G8,150);DelayMs(50);
				BuzzerTone(TONE_G8,150);DelayMs(50);
				BuzzerTone(TONE_A8,50);DelayMs(50);
				BuzzerTone(TONE_A8,50);DelayMs(50);
				BuzzerTone(TONE_A8,50);DelayMs(50);
				BuzzerTone(TONE_A8,50);DelayMs(50);
				BuzzerTone(TONE_G8,400);
				BuzzerTone(TONE_A8,50);DelayMs(50);
				BuzzerTone(TONE_A8,50);DelayMs(50);
				BuzzerTone(TONE_A8,50);DelayMs(50);
				BuzzerTone(TONE_A8,50);DelayMs(50);
				BuzzerTone(TONE_G8,400);
				BuzzerTone(TONE_F8,50);DelayMs(50);
				BuzzerTone(TONE_F8,50);DelayMs(50);
				BuzzerTone(TONE_F8,50);DelayMs(50);
				BuzzerTone(TONE_F8,50);DelayMs(50);
				BuzzerTone(TONE_E8,150);DelayMs(50);
				BuzzerTone(TONE_E8,150);DelayMs(50);
				BuzzerTone(TONE_D8,50);DelayMs(50);
				BuzzerTone(TONE_D8,50);DelayMs(50);
				BuzzerTone(TONE_D8,50);DelayMs(50);
				BuzzerTone(TONE_D8,50);DelayMs(50);
				BuzzerTone(TONE_C8,400);
				break;
		case 4:
				//fanfare
				BuzzerTone(TONE_C6,100);
				BuzzerTone(TONE_E6,100);
				BuzzerTone(TONE_G6,100);
				BuzzerTone(TONE_C7,200);
				BuzzerTone(TONE_G6,100);
				BuzzerTone(TONE_C7,500);
				break;
		case 5:
				//Deutschland Nationalhymne
				BuzzerTone(TONE_C5,600);
				BuzzerTone(TONE_D5,200);
				BuzzerTone(TONE_E5,400);
				BuzzerTone(TONE_D5,400);
				BuzzerTone(TONE_F5,400);
				BuzzerTone(TONE_E5,400);
				BuzzerTone(TONE_D5,200);
				BuzzerTone(TONE_H4,200);
				BuzzerTone(TONE_C5,400);

				BuzzerTone(TONE_A5,400);
				BuzzerTone(TONE_G5,400);
				BuzzerTone(TONE_F5,400);
				BuzzerTone(TONE_E5,400);
				BuzzerTone(TONE_D5,400);

				BuzzerTone(TONE_E5,200);
				BuzzerTone(TONE_C5,200);
				BuzzerTone(TONE_G5,800);

				break;
	}
}

