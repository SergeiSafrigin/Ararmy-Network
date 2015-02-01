/*
HMC5883L.cpp - Class file for the HMC5883L Triple Axis Magnetometer Arduino Library.
Copyright (C) 2011 Love Electronics (loveelectronics.co.uk)

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

 WARNING: THE HMC5883L IS NOT IDENTICAL TO THE HMC5883!
 Datasheet for HMC5883L:
 http://www51.honeywell.com/aero/common/documents/myaerospacecatalog-documents/Defense_Brochures-documents/HMC5883L_3-Axis_Digital_Compass_IC.pdf
 http://c48754.r54.cf3.rackcdn.com/HMC5883L.pdf

*/
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#include "Wire.h"
#define printByte(args) Serial.write(args)
#define printlnByte(args)  Serial.write(args),Serial.println()

#define write_I2C(args) Wire.write(args)
#define read_I2C() Wire.read()

#else
#include "WProgram.h"
#define printByte(args) Serial.print(args,BYTE)
#define printlnByte(args)  Serial.println(args,BYTE)

#define write_I2C(args) Wire.send(args)
#define read_I2C() Wire.receive()

#endif

#include "HMC5883L.h"

HMC5883L::HMC5883L()
{
	m_Scale = 0.92;
}

MagnetometerRaw HMC5883L::ReadRawAxis()
{
  uint8_t* buffer = Read(DataRegisterBegin, 6);
  MagnetometerRaw raw = MagnetometerRaw();
  raw.XAxis = (buffer[0] << 8) | buffer[1];
  raw.ZAxis = (buffer[2] << 8) | buffer[3];
  raw.YAxis = (buffer[4] << 8) | buffer[5];
  return raw;
}

MagnetometerScaled HMC5883L::ReadScaledAxis()
{
  MagnetometerRaw raw = ReadRawAxis();
  MagnetometerScaled scaled = MagnetometerScaled();
  scaled.XAxis = raw.XAxis * m_Scale;
  scaled.ZAxis = raw.ZAxis * m_Scale;
  scaled.YAxis = raw.YAxis * m_Scale;
  return scaled;
}

void HMC5883L::SetData()
{	
	Write(ModeRegister, Measurement_Continuous);
	uint8_t regValue = 0x01 << 5;
	Write(ConfigurationRegisterB, regValue);
}

uint8_t HMC5883L::EnsureConnected()
{
	uint8_t data = Read(IdentityRegister, 1)[0];

	if(data == IdentityRegisterValue)
		IsConnected = 1;
	else
		IsConnected = 0;

	return IsConnected;
}

void HMC5883L::Write(int address, int data)
{
  Wire.beginTransmission(HMC5883L_Address);
  write_I2C(address);
  write_I2C(data);
  Wire.endTransmission();
}

uint8_t* HMC5883L::Read(int address, int length)
{
  Wire.beginTransmission(HMC5883L_Address);
  write_I2C(address);
  Wire.endTransmission();
  
  Wire.beginTransmission(HMC5883L_Address);
  Wire.requestFrom(HMC5883L_Address, length);

  uint8_t buffer[length];
  if(Wire.available() == length)
  {
	  for(uint8_t i = 0; i < length; i++)
	  {
		  buffer[i] = read_I2C();
	  }
  }
  Wire.endTransmission();

  return buffer;
}

char* HMC5883L::GetErrorText(int errorCode)
{
	if(ErrorCode_1_Num == 1)
		return ErrorCode_1;
	
	return "Error not defined.";
}