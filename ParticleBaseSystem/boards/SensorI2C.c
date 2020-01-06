/**
* I2C driver for sensor i2c bus
*
* Author: krohn@teco.edu
* Date: 2003-12-01
*
* modified by cdecker@teco.edu
* Date: 2003-12-01
*
* version: 001 (2003-12-01, krohn)
*   - first implementation for fast version
*
* version: 001 (2003-12-01, krohn)
*   - modification for i2c sensors
*
* version: 002 (2004-04-08, sabin)
* 	- #use i2c without 'slow'
*
*/


#ifndef __SENSORI2C_C__
#define __SENSORI2C_C__				002


/**
* Initializes the sensor i2c bus.
* Sets tristates and powers up all i2c sensor devices
*/
void SensorI2CInit();

/**
* Switches all i2c sensor devices on
*/
void SensorI2COn();

/**
* Switches all i2c sensor devices off
*/
void SensorI2COff();

/**
* Raises i2c start condition on the sensor i2c bus
*/
#separate
void SensorI2CStart();

/**
* Writes a byte on the sensor i2c bus
* @param data byte to write on the bus
* @return a byte contaning the ACK bit
*/
#separate
byte SensorI2CWrite(byte data);

/**
* Reads a byte from the sensor i2c bus
* @param data parameter to indicate that the function will not ACK the received data
* @return a byte read from the sensor i2c bus
*/
#separate
byte SensorI2CRead(byte data);

/**
* Raises i2c stop condition on the sensor i2c bus
*/
#separate
void SensorI2CStop();




void SensorI2CInit()
{
        bit_set(TRIS_SENSOR_I2C_SDA);               //set data input
        bit_set(TRIS_SENSOR_I2C_SCL);               //set clock to input
        bit_clear(TRIS_SENSOR_I2C_POWER);   //set power to output
        output_high(PIN_SENSOR_I2C_POWER);              //set default: on!
}


void SensorI2COn()
{
        output_high(PIN_SENSOR_I2C_POWER);
}


void SensorI2COff()
{
        output_low(PIN_SENSOR_I2C_POWER);
}


#separate
void SensorI2CStart()
{
        #use i2c(master, sda=PIN_SENSOR_I2C_SDA, scl= PIN_SENSOR_I2C_SCL)
        i2c_start();
}


#separate
byte SensorI2CWrite(byte data)
{
        #use i2c(master, sda=PIN_SENSOR_I2C_SDA, scl= PIN_SENSOR_I2C_SCL)
        return i2c_write(data);
}



#separate
byte SensorI2CRead(byte data)
{
        #use i2c(master, sda=PIN_SENSOR_I2C_SDA, scl= PIN_SENSOR_I2C_SCL)
        return i2c_read(data);
}


#separate
void SensorI2CStop()
{
        #use i2c(master, sda=PIN_SENSOR_I2C_SDA, scl= PIN_SENSOR_I2C_SCL)
        i2c_stop();
}


#endif //__SENSORI2C_C__