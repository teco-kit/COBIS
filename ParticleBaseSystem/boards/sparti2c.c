// Driver for i2c for eeprom on spart.
// 2004-01-21 adapted to SPart by sabin (wendhack@teco.edu)



void SPartI2CInit()
{
	bit_set(TRIS_I2C_SPART_SDA);		//set data input
	bit_set(TRIS_I2C_SPART_SCL);		//set clock to input
	bit_clear(TRIS_I2C_SPART_POWER);	//set power to output
	output_high(PIN_I2C_SPART_POWER);		//set default: on!
}

void SPartI2COn()
{
	output_high(PIN_I2C_SPART_POWER);
}

void SPartI2COff()
{
	output_low(PIN_I2C_SPART_POWER);
}





#separate
void SPartI2CStart()
{
	#use i2c(master, sda=PIN_I2C_SPART_SDA, scl= PIN_I2C_SPART_SCL,fast)
	i2c_start();

}

#separate
byte SPartI2CWrite(byte data)
{
	#use i2c(master, sda=PIN_I2C_SPART_SDA, scl= PIN_I2C_SPART_SCL,fast)
	return i2c_write(data);
}


#separate
byte SPartI2CRead(byte data)
{
	#use i2c(master, sda=PIN_I2C_SPART_SDA, scl= PIN_I2C_SPART_SCL,fast)
	return i2c_read(data);
}

#separate
void SPartI2CStop()
{
	#use i2c(master, sda=PIN_I2C_SPART_SDA, scl= PIN_I2C_SPART_SCL,fast)
	i2c_stop();
}


