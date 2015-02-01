#include <Wire.h>
//#include <FreeSixIMU.h> //orientation
//#include <FIMU_ADXL345.h> //acceleromator
//#include <FIMU_ITG3200.h> //gyro driver
//#include <HMC5883L.h> //compass
#include <BMP085.h> //barometor + tempature
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

const int buttonPinMinus = 10;
const int buttonPinPlus = 11;

const int redLed = 8;
const int greenLed = 9;

//static const int RXPin = 2, TXPin = 3;
//SoftwareSerial myCompass(RXPin, TXPin);

int RXXBee=4, TXXBee=5;
SoftwareSerial XBeeSS(RXXBee,TXXBee);

int RXBT=6, TXBT=7;
SoftwareSerial BTSS(RXBT, TXBT);

TinyGPSPlus gps;

//HMC5883L compass = HMC5883L();
//float heading; //real yaw. 0 to 360

//FreeSixIMU sixDOF = FreeSixIMU();
//float angles[3]; //angles[0] = ignore, angles[1] = roll, angles[2] = pitch

BMP085 dps = BMP085();
long Pressure = 0, Altitude = 0;

char pszGPSInfo[128];
char pszGYROInfo[128];

//float MovmentCheck[3];
int MovmentCheckCounter = 0;

String id = "s";
char gpsData[45];
char oriantationData[45];
String xBeeData;
String data;

float yaw, pitch, roll;

char headingBuff[14], yawBuff[14], pitchBuff[14], rollBuff[14], latBuff[14], lonBuff[14], altBuff[14];

void setup()
{ 
  Serial.begin(9600);
  Serial.println("Loading..");
  pinMode(buttonPinMinus, INPUT);
  pinMode(buttonPinPlus, OUTPUT);
  digitalWrite(buttonPinPlus, HIGH);
  
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  
  BTSS.begin(9600);
  XBeeSS.begin(9600);
  //myCompass.begin(9600);
    
  Wire.begin();
  delay(1000);
  
  dps.init();
  delay(5000);
    
  //sixDOF.init(); //init the Acc and Gyro
  //delay(5);
    
  //compass.SetData();
  
  strcpy(gpsData, "0,0,0;");
    
  Serial.println("Done.");
  Serial.println("id, yaw, pitch, roll, pressure, lat, lon, alt;civilId, lat, lon, alt;...");
}


void loop()
{
  xBeeData = "";
 
  dps.getPressure(&Pressure);
  dps.getAltitude(&Altitude);
   
  while (Serial.available() > 0) {
    if (gps.encode(Serial.read())) {
      sprintf(gpsData, "%s,%s,%s;",
        (gps.location.isValid()?floatToString(gps.location.lat(), 6, latBuff) : "0"),
        (gps.location.isValid()?floatToString(gps.location.lng(), 6, lonBuff) : "0"),
        (gps.altitude.isValid()?floatToString(gps.altitude.meters(), 2, altBuff) : "0")
      );
    }      
  }
    
  if (gps.location.isValid())
    digitalWrite(greenLed, HIGH);
  else
    digitalWrite(greenLed, LOW);
  
  if (digitalRead(buttonPinMinus) == HIGH) {
    
    XBeeSS.print("1");
    delay(1000);
    
    while(XBeeSS.available()) {
      char c = XBeeSS.read();
      xBeeData += c;
    }
    
//    sprintf(oriantationData, "%s,%s,%s,%s,",
//      floatToString(heading, 4, headingBuff),
//      floatToString(angles[2], 2, pitchBuff),
//      floatToString(angles[1], 2, rollBuff)
//    );  

//    sprintf(oriantationData, "%s,%s,%s,",
//      floatToString(yaw, 4, headingBuff),
//      floatToString(pitch, 4, pitchBuff),
//      floatToString(roll, 2, rollBuff)
//    );  
    
    data = (id + "," + Pressure + "," + gpsData);
    if (xBeeData.length() > 0) {
      data += xBeeData;
      digitalWrite(redLed, HIGH);
    }
    
    Serial.println(data);
    BTSS.println(data);
    digitalWrite(redLed, LOW);
  }
    
  delay(200);
}

char* floatToString(float num, int prec, char* buffer) {
  return dtostrf(num, 0, prec, buffer);
}

//void step_of_MUI(){
//  dps.getPressure(&Pressure);
//  dps.getAltitude(&Altitude);
//  sixDOF.getEuler(angles);
//  getHeading();
  
//  myCompass.write(":3\n");
//  
//  delay(100);
//  
//  String data2 = "";
//
//  while (myCompass.available()) {
//    char c = myCompass.read();
//    data2 += c;
//    Serial.print("-");
//  }
//  
//  Serial.println(data2);
//
//  int delimIndex = data2.indexOf(",");
//  pitch = data2.substring(0, delimIndex).toFloat() * 180/M_PI;
//  data = data2.substring(delimIndex+1);
//  delimIndex = data2.indexOf(",");
//  yaw = data2.substring(0, delimIndex).toFloat() * 180/M_PI;
//  roll = data2.substring(delimIndex+1, data2.indexOf('\n')).toFloat() * 180/M_PI;  
//}

//void getHeading(){
//  MagnetometerScaled scaled = compass.ReadScaledAxis();
//  heading = atan2(scaled.YAxis, scaled.XAxis) + 0.0457;
//  
//  if(heading < 0)
//    heading += 2*PI;
//  else if(heading > 2*PI)
//    heading -= 2*PI;
//   
//  heading *= 180/M_PI;
//  
//  if (heading > 360)
//    heading -= 360;
//}

//void step_of_XBee()
//{
//  Civil1 = "";
//    
//  while(XBeeSS.available()) {
//    character = XBeeSS.read();
//    Civil1.concat(character);
//  }
//  
//  Serial.println(Civil1);
//  
//  if (check_movement()) {
//    MovmentCheckCounter++;
//		
//    if(MovmentCheckCounter==50) {
//      BTSS.print(ID);
//      BTSS.print(", not moving, ");
//      BTSS.print(pszGYROInfo);
//      BTSS.print(pszGPSInfo);
//      BTSS.print(";");
//      BTSS.println(Civil1);
//    }
//  } else {
//    MovmentCheck[0]=angles[0];
//    MovmentCheck[1]=angles[1];
//    MovmentCheck[2]=angles[2];
//    MovmentCheckCounter=0;
//  }
//  
//  if (Civil1 == "1") {
//    Serial.println("Civil1 == 1");
//    XBeeSS.print(ID);
//    XBeeSS.print(", Answering, ");
//    XBeeSS.print(pszGYROInfo);
//    XBeeSS.print(pszGPSInfo);
//    XBeeSS.println(";");
//  } else if (digitalRead(buttonPin)==HIGH) {
//    Serial.println("button pressed");
//    XBeeSS.println("1");
//    while(XBeeSS.available()) {
//      character = XBeeSS.read();
//      Civil1.concat(character);
//    }
//		
//    BTSS.print(ID);
//    BTSS.print(", Checking, ");
//    BTSS.print(pszGYROInfo);
//    BTSS.print(pszGPSInfo);
//    BTSS.print(";");  
//    BTSS.println(Civil1);
//  }
//}
//
//boolean check_movement()
//{
//  if (angles[0]<(MovmentCheck[0]+2) ||
//    angles[0]>(MovmentCheck[0]-2) ||
//    angles[1]<(MovmentCheck[1]+2) ||
//    angles[1]>(MovmentCheck[1]-2) ||
//    angles[2]<(MovmentCheck[2]+2) ||
//    angles[2]>(MovmentCheck[2]-2))
//    
//    return true;
//  return false;
//}
