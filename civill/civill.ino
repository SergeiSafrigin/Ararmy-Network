#include <Wire.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <BMP085.h>

int RXXBee=4, TXXBee=5;
SoftwareSerial XBeeSS(RXXBee,TXXBee);

int RXBT=6, TXBT=7;
SoftwareSerial BTSS(RXBT, TXBT);

TinyGPSPlus gps;

BMP085 dps = BMP085();
long Pressure = 0, Altitude = 0;

String id = "C1";
char gpsData[45];
String xBeeData;
String data;

char latBuff[14], lonBuff[14], altBuff[14];

void setup()
{ 
  Serial.begin(9600);
  Serial.println("Loading..");
  
  //BTSS.begin(9600);
  XBeeSS.begin(9600);
  
  Wire.begin();
  
  Serial.println("---");
  
  dps.init();
  delay(5000);
  
  strcpy(gpsData, "0,0,0;");
  
  Serial.println("Done.");
  Serial.println("id, lat, lon, alt;");
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
  
  while(XBeeSS.available()) {
    char c = XBeeSS.read();
    xBeeData += c;
  }
  
  data = (id + "," + Pressure + "," + gpsData);
  if (xBeeData.length() > 0) {
    Serial.println(("Received New Message: "+xBeeData));
    
    int xBeeInt = xBeeData.toInt();
    
    if (xBeeInt == 1)
      XBeeSS.print(data);
    else if (xBeeInt == 2 || xBeeInt == 3)
      BTSS.print(data);
  }
    
  delay(200);
}

char* floatToString(float num, int prec, char* buffer) {
  return dtostrf(num, 0, prec, buffer);
}
