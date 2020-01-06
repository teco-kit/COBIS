/**
* driver for new ballswitch on pc228
*
* Author: krohn@teco.edu
* Date: 2004-08-12
*
* file version: 002 (2004-09-07, stefan)
* BallswitchGetPosition() removed, only one INNER
*/


#define __BALLSWITCH_C__ 002





// initialize the ballswitch to normal interrupt mode
void BallswitchInit()
{
	//outer on input, inner all high

	bit_set(TRIS_BALL_OUTER);

	bit_clear(TRIS_BALL_INNER);
	output_high(PIN_BALL_INNER);

}


//sets the pins that the b0 interrupt can be used externally
void BallswitchDisable()
{
	bit_set(TRIS_BALL_OUTER);
	bit_set(TRIS_BALL_INNER);

}




//returns 1 or 0
int BallswitchGet()
{

	if (input(PIN_BALL_OUTER)) return 1; else return 0;
}






