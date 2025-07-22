
#include <Wire.h>
#define sda 21
#define scl 22
#define ADS1115_ADDRESS 0x48

int16_t test_val = 0x15;
float voltage;
const int alrt = 15 ; //declare the alert pin 
volatile bool conversion_rdy = false;
uint16_t config ;


//register address
#define ADS1X15_REG_POINTER_CONVERT 0x00
#define ADS1X15_REG_POINTER_CONFIG  0x01
#define ADS1X15_REG_POINTER_LOWTHRESH  0x02
#define ADS1X15_REG_POINTER_HITHRESH   0x03

bool isI2CDeviceConnected(uint8_t address) 
{
  Wire.beginTransmission(address);
  return (Wire.endTransmission() == 0);
}

void writeAddress(uint8_t reg,uint16_t value)     //function to write the instruction 
{
  Wire.beginTransmission(ADS1115_ADDRESS);
  Wire.write(reg);
  Wire.write((value >>8)& 0xFF);
  Wire.write(value & 0xFF);
  Wire.endTransmission();
}


int readAddress(uint8_t reg)                     //function to read the data
{
  
  Wire.beginTransmission(ADS1115_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(ADS1115_ADDRESS,2);
  return ((Wire.read() <<8) | Wire.read());                         

}

void data_conv_ISR()                                     //interrupt  
{
  conversion_rdy = true;
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Initiallising.....");
  pinMode(alrt , INPUT);
  attachInterrupt(digitalPinToInterrupt(alrt),data_conv_ISR,FALLING);

  
  Wire.begin(sda,scl);
  if(isI2CDeviceConnected(ADS1115_ADDRESS))
  {
    Serial.println("Device found");
    writeAddress(ADS1X15_REG_POINTER_LOWTHRESH, 0x0000);
    writeAddress(ADS1X15_REG_POINTER_HITHRESH, 0x8000);
  }





  config =  0x8000 |  // OS: start single conversion
            0x4000 |  // MUX: AIN0
            0x0200 |  // PGA: ±4.096V
            0x0100 |  // MODE: single shot
            0x0080 |  // DR: 128 SPS
            0x0000 |  // CMODE: traditional comparator
            0x0000 |  // CPOL: active low
            0x0000 |  // CLAT: non-latching
            0x0000;

  
}

void loop()
{
  Serial.println("Commencing");
  writeAddress(ADS1X15_REG_POINTER_CONFIG ,config);
  if(conversion_rdy)
  {
  test_val = readAddress(ADS1X15_REG_POINTER_CONVERT);
  float voltage = test_val * 0.125 / 1000.0;
  Serial.println(voltage);
  }
  delay(1000);

  
}
