// TECO Michael Beigl Oct 2003

// Header file for pic 18F6x2x
//
// containing the registers for the pic


// Hardware Adresses

#define PROCESSOR_TYPE	186720

#byte	PIC_TMR4		= 0xF78
#byte	PIC_PR4			= 0xF77
#byte	PIC_T4CON		= 0xF76
#byte	PIC_CCPR4H		= 0xF75
#byte	PIC_CCP4CON		= 0xF74
#byte	PIC_CCPR5H		= 0xF72
#byte	PIC_CCPR5L		= 0xF71
#byte	PIC_CCP5CON		= 0xF70
#byte	PIC_SPBRG2		= 0xF6F
#byte	PIC_RCREG2		= 0xF6E
#byte	PIC_TXREG2		= 0xF6D
#byte	PIC_TXSTA2		= 0xF6C
#byte	PIC_RCSTA2		= 0xF6B

#byte   PIC_PORTA		= 0xF80
#byte   PIC_PORTB		= 0xF81
#byte   PIC_PORTC		= 0xF82
#byte   PIC_PORTD		= 0xF83
#byte   PIC_PORTE		= 0xF84
#byte	PIC_PORTF		= 0xF85
#byte   PIC_PORTG		= 0xF86
#byte   PIC_LATA		= 0xF89
#byte   PIC_LATB		= 0xF8A
#byte   PIC_LATC		= 0xF8B
#byte   PIC_LATD		= 0xF8C
#byte   PIC_LATE		= 0xF8D
#byte	PIC_LATF	 	= 0xF8E
#byte   PIC_TRISA		= 0xF92
#byte   PIC_TRISB		= 0xF93
#byte   PIC_TRISC		= 0xF94
#byte   PIC_TRISD		= 0xF95
#byte   PIC_TRISE		= 0xF96
#byte   PIC_TRISF		= 0xF97
#byte   PIC_TRISG		= 0xF98

#byte   PIC_PIE1		= 0xF9D
#byte   PIC_PIR1		= 0xF9E
#byte   PIC_IPR1		= 0xF9F
#byte   PIC_PIE2		= 0xFA0
#byte   PIC_PIR2		= 0xFA1
#byte   PIC_IPR2		= 0xFA2
#byte   PIC_PIE3		= 0xFA3
#byte   PIC_PIR3		= 0xFA4
#byte   PIC_IPR3		= 0xFA5

#byte   PIC_EECON1		= 0xFA6
#byte   PIC_EECON2		= 0xFA7
#byte   PIC_EEDATA		= 0xFA8
#byte   PIC_EEADR		= 0xFA9

#byte   PIC_RCSTA1		= 0xFAB
#byte   PIC_TXSTA1		= 0xFAC
#byte   PIC_TXREG1		= 0xFAD
#byte   PIC_RCREG1		= 0xFAE
#byte   PIC_SPBRG1		= 0xFAF

#byte   PIC_T3CON		= 0xFB1
#byte   PIC_TMR3L		= 0xFB2
#byte   PIC_TMR3H		= 0xFB3

#byte   PIC_CCP2CON		= 0xFBA
#byte   PIC_CCPR2L		= 0xFBB
#byte   PIC_CCPR2H		= 0xFBC
#byte   PIC_CCP1CON		= 0xFBD
#byte   PIC_CCPR1L		= 0xFBE
#byte   PIC_CCPR1H		= 0xFBF

#byte 	PIC_ADCON2		= 0xFC0
#byte   PIC_ADCON1		= 0xFC1
#byte   PIC_ADCON0		= 0xFC2
#byte   PIC_ADRESL		= 0xFC3
#byte   PIC_ADRESH		= 0xFC4
#byte   PIC_SSPCON2		= 0xFC5
#byte   PIC_SSPCON1		= 0xFC6
#byte   PIC_SSPSTAT		= 0xFC7
#byte   PIC_SSPADD		= 0xFC8
#byte   PIC_SSPBUF		= 0xFC9
#byte   PIC_T2CON		= 0xFCA
#byte   PIC_PR2			= 0xFCB
#byte   PIC_TMR2		= 0xFCC
#byte   PIC_T1CON		= 0xFCD
#byte   PIC_TMR1L		= 0xFCE
#byte   PIC_TMR1H		= 0xFCF
#byte   PIC_RCON		= 0xFD0
#byte   PIC_WDTCON		= 0xFD1
#byte   PIC_LVDCON		= 0xFD2
#byte   PIC_OSCCON		= 0xFD3
#byte   PIC_T0CON		= 0xFD5
#byte   PIC_TMR0L		= 0xFD6
#byte   PIC_TMR0H		= 0xFD7
#byte   PIC_STATUS		= 0xFD8
#byte   PIC_FSR2L		= 0xFD9
#byte   PIC_FSR2H		= 0xFDA
#byte   PIC_PLUSW2		= 0xFDB
#byte   PIC_PREINC2		= 0xFDC
#byte   PIC_POSTDEC2	= 0xFDD
#byte   PIC_POSTINC2	= 0xFDE
#byte   PIC_INDF2		= 0xFDF
#byte   PIC_BSR			= 0xFE0
#byte   PIC_FSR1L		= 0xFE1
#byte   PIC_FSR1H		= 0xFE2
#byte   PIC_PLUSW1		= 0xFE3
#byte   PIC_PREINC1		= 0xFE4
#byte   PIC_POSTDEC1	= 0xFE5
#byte   PIC_POSTINC1	= 0xFE6
#byte   PIC_INDF1		= 0xFE7
#byte   PIC_WREG		= 0xFE8
#byte   PIC_FSR0L		= 0xFE9
#byte   PIC_FSR0H		= 0xFEA
#byte   PIC_PLUSW0		= 0xFEB
#byte   PIC_PREINC0		= 0xFEC
#byte   PIC_POSTDEC0	= 0xFED
#byte   PIC_POSTINC0	= 0xFEE
#byte   PIC_INDF0		= 0xFEF
#byte   PIC_INTCON3		= 0xFF0
#byte   PIC_INTCON2		= 0xFF1
#byte	PIC_INTCON		= 0xFF2
#byte   PIC_INCON		= 0xFF2
#byte   PIC_PRODL		= 0xFF3
#byte   PIC_PRODH		= 0xFF4
#byte   PIC_TABLAT		= 0xFF5
#byte   PIC_TBLPTRL		= 0xFF6
#byte   PIC_TBLPTRH		= 0xFF7
#byte   PIC_TBLPTRU		= 0xFF8
#byte   PIC_PCL			= 0xFF9
#byte   PIC_PCLATH		= 0xFFA
#byte   PIC_PCLATU		= 0xFFB
#byte   PIC_STKPTR		= 0xFFC
#byte   PIC_TOSL		= 0xFFD
#byte   PIC_TOSH		= 0xFFE
#byte   PIC_TOSU		= 0xFFF




//for EEPROM access
//#define EEADR 				0xFA9
//#define EEDATA 				0xFA8
//#define EECON1 				0xFA6
#define RD 					0
#define WR 					1
#define WREN 				2
#define WRERR 				3
#define FREE 				4
#define CFGS 				6
#define EEPGD 				7
//#define EECON2 				0xFA7
//#define PIR2 				0xFA1
#define EEIF 				4


#define ADCH_PIN_A0					 0b00000000
#define ADCH_PIN_A1					 0b00000001
#define ADCH_PIN_A2                  0b00000010
#define ADCH_PIN_A3                  0b00000011
#define ADCH_PIN_A5                  0b00000100
#define ADCH_PIN_F0                  0b00000101