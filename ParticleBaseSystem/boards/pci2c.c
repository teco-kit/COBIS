// new i2c_pc
// defines must be set in pc202.c



void PCI2CInit()
{
	bit_set(TRIS_I2C_PC_SDA);		//set data input
	bit_set(TRIS_I2C_PC_SCL);		//set clock to input
	bit_clear(TRIS_I2C_PC_POWER);	//set power to output
	output_high(PIN_I2C_PC_POWER);		//set default: on!
}

void PCI2COn()
{
	output_high(PIN_I2C_PC_POWER);
}

void PCI2COff()
{
	output_low(PIN_I2C_PC_POWER);
}





#separate
void PCI2CStart()
{
	bit_set(TRIS_I2C_PC_SDA);		//set data input
	bit_set(TRIS_I2C_PC_SCL);		//set clock to input

	#use i2c(master, sda=PIN_I2C_PC_SDA, scl= PIN_I2C_PC_SCL,fast)
	i2c_start();

}

#separate
byte PCI2CWrite(byte data)
{
	#use i2c(master, sda=PIN_I2C_PC_SDA, scl= PIN_I2C_PC_SCL,fast)
	return i2c_write(data);
}


#separate
byte PCI2CRead(byte data)
{
	#use i2c(master, sda=PIN_I2C_PC_SDA, scl= PIN_I2C_PC_SCL,fast)
	return i2c_read(data);
}

#separate
void PCI2CStop()
{
	#use i2c(master, sda=PIN_I2C_PC_SDA, scl= PIN_I2C_PC_SCL,fast)
	i2c_stop();
	bit_clear(TRIS_I2C_PC_SDA);		//set data output, since RF_TX == I2C_PC_SDA
}


