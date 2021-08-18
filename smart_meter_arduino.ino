#include <GPRSClient.h>
#include <Xively.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include "RTClib.h"
#include <Eeprom24C32_64.h>
#define EEPROM_ADDRESS 0x50
#define PIN_TX 7
#define PIN_RX 8
static Eeprom24C32_64 eeprom(EEPROM_ADDRESS);
long int aa;
RTC_DS1307 RTC;
char xivelyKey[] = "nZXwRV6CFycsLlu6RHftBm2JyANna1laF0T3EOOQC5Gppdw5";
char sensorId[] = "voltage";
char sensorId1[] = "current";
char sensorId2[] = "power";
XivelyDatastream datastreams[] = {
XivelyDatastream(sensorId, strlen(sensorId), DATASTREAM_FLOAT),XivelyDatastream(sensorId1, strlen(sensorId1), DATASTREAM_FLOAT),XivelyDatastream(sensorId2, strlen(sensorId2), DATASTREAM_FLOAT),};
XivelyFeed feed(1438036627, datastreams, 3 /* number of datastreams */);
char apn[] = "airtelgprs.com";
GPRSClient gprs(PIN_TX,PIN_RX,9600,apn,NULL,NULL);
XivelyClient xivelyclient(gprs);
const int sensorV = A0;
const int sensorC = A1;
int mVperAmp = 100;
double Voltage = 0;
double Current=0;
double VRMS = 0;
double AmpsRMS = 0;
double VolRMS = 0;
float Power=0;
float totalpower=0;
float totalpow=0;
void setup() {
Serial.begin(9600);
Wire.begin();
RTC.begin();
eeprom.initialize();
if (! RTC.isrunning()) {
Serial.println("RTC is NOT running!");
}
float tempread;
tempread=eepromread();
totalpow=(tempread/10);
Serial.println("Starting datastream upload to Xively...");
Serial.println();
gprs.init();
while(false == gprs.join()) {
Serial.println("connect error");
delay(2000);
}
Serial.print("IP = ");
Serial.println(gprs.getIPAddress());
}
void loop() {
  Serial.print("Read byte = ");
  Serial.print(totalpow);
  Current = getVPP();
  VRMS = (Current/2.0) *0.707;
  AmpsRMS = (VRMS * 1000)/mVperAmp;
  Voltage= getVol();
  VolRMS = Voltage;
  Power = ((AmpsRMS*VolRMS));
  eepromwrite();
  aa=getpower()*10 ;
  DateTime now = RTC.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.print(AmpsRMS);
  Serial.println(" A");
  Serial.print(VolRMS);
  Serial.println("V");
  Serial.print(Power);
  Serial.println("W");
  datastreams[0].setFloat(VolRMS);
  datastreams[1].setFloat(AmpsRMS);
  datastreams[2].setFloat(Power);
  Serial.print("Read sensor value ");
  Serial.println(datastreams[0].getFloat());
  Serial.println(datastreams[1].getFloat());
  Serial.println(datastreams[2].getFloat());
  Serial.println("Uploading it to Xively");
  int ret = xivelyclient.put(feed, xivelyKey);
  if(true == ret){
  Serial.println("update success!");
  }else{
  Serial.println("update failed!");
  }
  Serial.println();
  delay(1000);
}
float getVPP()
{
  float result;
  int readValue; //value read from the sensor
  int maxValue = 0; // store max value here
  int minValue = 1024; // store min value here
  uint32_t start_time = millis();
  while((millis()-start_time) < 300) //sample for 1 Sec
    {
    readValue = analogRead(sensorC);
    // see if you have a new maxValue
    if (readValue > maxValue)
      {
      maxValue = readValue; /*record the maximum sensor value*/
      }
    if (readValue < minValue)
      {
      minValue = readValue; /*record the maximum sensor value*/
      }
    }
  result = (((maxValue - minValue)-2.8) * 5.0)/1024.0; // Subtract min from max
  return result;
}
float getVol()
{
  float result1=0;
  int readValue1= 0; //value read from the sensor
  uint32_t start_time = millis();
  while((millis()-start_time) < 300) //sample
    {
    readValue1 = analogRead(sensorV);
    }
  result1 = (((readValue1) * 5.0)/1024.0)*86.219;
  return result1;
}
float getpower()
{
  float Current1 = getVPP();
  float VRMS1 = (Current1/2.0) *0.707;
  float AmpsRMS1 = (VRMS1 * 1000)/mVperAmp;
  float Voltage1= getVol();
  float VolRMS1 = Voltage1;
  uint32_t start_time = millis();
  while((millis()-start_time) < 1000) //
    {
    totalpower=((AmpsRMS1*VolRMS1)/3600);
    }
  totalpow += totalpower ;
  return totalpow;
}
void eepromwrite()
{
  int bb = aa/1000000;
  int b1=bb/10;
  int b2=bb%10;
  int cc=aa/10000;
  int dd=cc%100;
  int d1=dd/10;
  int d2=dd%10;
  long int ee = aa%10000;
  int ff=ee/100;
  int f1=ff/10;
  int f2=ff%10;
  int gg=aa%100;
  int g1=gg/10;
  int g2=gg%10;
  const word address0 = 0x00;
  const word address1 = 0x01;
  const word address2 = 0x02;
  const word address3 = 0x03;
  const word address4 = 0x04;
  const word address5 = 0x05;
  const word address6 = 0x06;
  const word address7 = 0x08;
  int dataa0 = b1;
  int dataa1 = b2;
  int dataa2 = d1;
  int dataa3 = d2;
  int dataa4 = f1;
  int dataa5 = f2;
  int dataa6 = g1;
  int dataa7 = g2;
  // Write a byte at address 0 in EEPROM memory.
  Serial.println("Write byte to EEPROM memory...");
  eeprom.writeByte(address0,dataa0);
  delay(10);
  eeprom.writeByte(address1,dataa1);
  delay(10);
  eeprom.writeByte(address2,dataa2); // Write cycle time (tWR). See EEPROM memory datasheet for more details.
  delay(10);
  eeprom.writeByte(address3,dataa3);
  delay(10);
  eeprom.writeByte(address4,dataa4);
  delay(10);
  eeprom.writeByte(address5,dataa5);
  delay(10);
  eeprom.writeByte(address6,dataa6);
  delay(10);
  eeprom.writeByte(address7,dataa7);
  delay(10);
}
float eepromread()
{ 
  const word address0 = 0x00;
  const word address1 = 0x01;
  const word address2 = 0x02;
  const word address3 = 0x03;
  const word address4 = 0x04;
  const word address5 = 0x05;
  const word address6 = 0x06;
  const word address7 = 0x08;
  Serial.println("Read byte from EEPROM memory...");
  int myArray[] = {eeprom.readByte(address0),eeprom.readByte(address1),eeprom.readByte(address2),eeprom.readByte(address3),
  eeprom.readByte(address4),eeprom.readByte(address5),eeprom.readByte(address6),eeprom.readByte(address7)};
  float aa0=myArray[0]; float aaa0=aa0*10000000;
  float aa1=myArray[1];float aaa1=aa1*1000000;
  float aa2=myArray[2];float aaa2=aa2*100000;
  float aa3=myArray[3];float aaa3=aa3*10000;
  float aa4=myArray[4];float aaa4=aa4*1000;
  float aa5=myArray[5];float aaa5=aa5*100;
  float aa6=myArray[6];float aaa6=aa6*10;
  float aa7=myArray[7];float aaa7=aa7*1;
  float final_value=aaa0+aaa1+aaa2+aaa3+aaa4+aaa5+aaa6+aaa7;
  return final_value;
}
