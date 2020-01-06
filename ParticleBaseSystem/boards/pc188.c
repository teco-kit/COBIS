/**
*
*	Boardheaderfile vor pc188; this board is designed for 18f452!!!
*
**/



#use delay(clock=20000000)






#define PIN_RF_CTR0 		PIN_D1
#define TRIS_RF_CTR0		PIC_TRISD,1
#define PIN_RF_CTR1 		PIN_D2
#define TRIS_RF_CTR1 		PIC_TRISD,2
#define PIN_RF_TX   		PIN_C5			// SPI Out
#define TRIS_RF_TX			PIC_TRISC,5
#define PIN_RF_RX  			PIN_G2			// Data Line of Usart in Synchronous Mode
#define TRIS_RF_RX			PIC_TRISC,7
#define PIN_RF_POWER		PIN_D3
#define TRIS_RF_POWER		PIC_TRISD,3
#define PIN_RF_SENSITIVE 	PIN_A4
#define TRIS_RF_SENSITIVE	PIC_TRISA,4


#define PIN_POTI_POWER		PIN_G3  //
#define TRIS_POTI_POWER		PIN_TRISG,3
#define PIN_POTI_UD			PIN_E0
#define TRIS_POTI_UD		PIC_TRISE,0
#define PIN_POTI_INC		PIN_G0
#define TRIS_POTI_INC		PIC_TRISG,0



#define LED_PC_RED			PIN_C2
#define LED_PC_BLUE			PIN_F6
#define RED					LED_PC_RED			//for convenience
#define BLUE				LED_PC_BLUE


//pc i2c for eeprom etc
#define PIN_I2C_PC_SDA		PIN_D6
#define TRIS_I2C_PC_SDA		PIC_TRISD,6
#define PIN_I2C_PC_SCL		PIN_D3
#define TRIS_I2C_PC_SCL		PIC_TRISD,3
#define	PIN_I2C_PC_POWER	PIN_D7
#define	TRIS_I2C_PC_POWER	PIC_TRISD,7

#define	XROM_ID				0



//connectors defines for conan
#define PIN_CONN_01                             // VCC
#define PIN_CONN_02                             PIN_B0
#define PIN_CONN_03                             PIN_B1
#define PIN_CONN_04                             PIN_B2
#define PIN_CONN_05                             PIN_B3
#define PIN_CONN_06                             PIN_B4
#define PIN_CONN_07                             PIN_B5
#define PIN_CONN_08                             PIN_B6
#define PIN_CONN_09                             PIN_B7
#define PIN_CONN_10                             // MCLR
#define PIN_CONN_11                             PIN_A2
#define PIN_CONN_12                             PIN_A3
#define PIN_CONN_13                             PIN_F0
#define PIN_CONN_14                             PIN_A5
#define PIN_CONN_15                             PIN_D6
#define PIN_CONN_16                             PIN_D3
#define PIN_CONN_17                             PIN_D4
#define PIN_CONN_18                             PIN_D5
#define PIN_CONN_19                             PIN_C6
#define PIN_CONN_20                             PIN_C7
#define PIN_CONN_21                             // GND

#define TRIS_CONN_01                    // VCC
#define TRIS_CONN_02                    PIC_TRISB,0
#define TRIS_CONN_03                    PIC_TRISB,1
#define TRIS_CONN_04                    PIC_TRISB,2
#define TRIS_CONN_05                    PIC_TRISB,3
#define TRIS_CONN_06                    PIC_TRISB,4
#define TRIS_CONN_07                    PIC_TRISB,5
#define TRIS_CONN_08                    PIC_TRISB,6
#define TRIS_CONN_09                    PIC_TRISB,7
#define TRIS_CONN_10                    // MCLR
#define TRIS_CONN_11                    PIC_TRISA,2
#define TRIS_CONN_12                    PIC_TRISA,3
#define TRIS_CONN_13                    PIC_TRISF,0
#define TRIS_CONN_14                    PIC_TRISA,5
#define TRIS_CONN_15                    PIC_TRISD,6
#define TRIS_CONN_16                    PIC_TRISD,3
#define TRIS_CONN_17                    PIC_TRISD,4
#define TRIS_CONN_18                    PIC_TRISD,5
#define TRIS_CONN_19                    PIC_TRISC,6
#define TRIS_CONN_20                    PIC_TRISC,7
#define TRIS_CONN_21                    // GND


#define ADCH_CONN_11                    0b00000010
#define ADCH_CONN_12                    0b00000011
#define ADCH_CONN_13                    0b00000100
#define ADCH_CONN_14                    0b00000101


//for RPC SERVER
#define TRIS_RPC_HANDSHAKE_SERVER		TRIS_CONN_04
#define PIN_RPC_HANDSHAKE_SERVER		PIN_CONN_04

//for RF; 18f6620
#define RF_TXSTA			PIC_TXSTA2
#define RF_SPBRG			PIC_SPBRG2
#define RF_RCREG			PIC_RCREG2
#define RF_RCSTA			PIC_RCSTA2
#define RF_UART_SREN		PIC_RCSTA2,5
#define RF_UART_CREN		PIC_RCSTA2,4
#define RF_UART_RCIF		PIC_PIR3,5
#define RF_UART_SPEN		PIC_RCSTA2,7

#define RF_SSPBUF			PIC_SSPBUF
#define RF_SSPSTAT_BF		PIC_SSPSTAT,0
#define RF_TIMER1_IF		PIC_PIR1,0
#define RF_TIMER1_CON		PIC_T1CON
#define RF_TIMER2_TMR2IF	PIC_PIR1,1

#define RF_TIMER1H			PIC_TMR1H
#define RF_TIMER1L			PIC_TMR1L
#define RF_TIMER2			PIC_TMR2

#define RF_SSPCON_SSPEN		PIC_SSPCON,5



#use fast_io(A)
#use fast_io(B)
#use fast_io(C)
#use fast_io(D)
#use fast_io(E)




/*
 *
 *	Power RF Module
 *

#inline
void RFPowerModuleOn()
{
	output_low(PIN_RF_POWER);	//inverse logic!!
	output_high(PIN_POTI_POWER);
}

#inline
void RFPowerModuleOff()
{
	output_high(PIN_RF_POWER);	//inverse logic!!
	output_low(PIN_POTI_POWER);
}
