// beacon.h
// Beacon-Board PIN definitions

// internal IR power control
#define INTERNAL_IR_STEP			PIN_CONN_04
#define INTERNAL_IR_DIR				PIN_CONN_03
#define INTERNAL_IR_ENABLE			PIN_CONN_07

#define TRIS_INTERNAL_IR_STEP		TRIS_CONN_04
#define TRIS_INTERNAL_IR_DIR		TRIS_CONN_03
#define TRIS_INTERNAL_IR_ENABLE		TRIS_CONN_07

// external IR power control
#define EXTERNAL_IR_STEP			PIN_CONN_06
#define EXTERNAL_IR_DIR				PIN_CONN_05
#define EXTERNAL_IR_ENABLE			PIN_CONN_08

// external IR power control
#define TRIS_EXTERNAL_IR_STEP		TRIS_CONN_06
#define TRIS_EXTERNAL_IR_DIR		TRIS_CONN_05
#define TRIS_EXTERNAL_IR_ENABLE		TRIS_CONN_08


// some other definitions
#define INTERNAL_IR_LED		1
#define EXTERNAL_IR_LED		0


// function def
void IRBoardInit();
void IRSetFieldStrength(unsigned int strength, unsigned int internal);
void IRsend38khz(byte b);
