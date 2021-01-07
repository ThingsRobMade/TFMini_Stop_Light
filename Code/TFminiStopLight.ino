/*
  TFMiniStopLight.ino
  Rob Reynolds, November 19, 2018

  This code is a small practical demonstration application
  of the TFMini Lidar Module. The 3D files can be found in
  the Github repository, here [ https://github.com/ThingsRobMade/TFMini_Stop_Light ]
  Based heavily on the previous collaborative work done by
  Nate Seidle and Benewake. The original example sketch for
  the Qwiic Enabled TFMini can be found here:
  (https://www.sparkfun.com/products/14786)

  This code is free, but if you find it useful, and we meet someday, you can buy me a beer (Beerware license).
  
*/

#include <Wire.h>

uint16_t distance = 0; //distance
uint16_t strength = 0; // signal strength
uint8_t rangeType = 0; //range scale
/*Value range:
  00 (short distance)
  03 (intermediate distance)
  07 (long distance) */

boolean valid_data = false; //ignore invalid ranging data

const byte sensor1 = 0x10; //TFMini I2C Address

// Define pins for LEDs
const int greenLED = 8;
const int yellowLED = 9;
const int redLED = 10;
int stopLimit = 160; //Change this number of cm to adjust stop distance

void setup()
{
  Wire.begin();

  Serial.begin(115200);
  Serial.println("TFMini I2C Test"); //For testing using Serial Monitor

  // Set LED pins as outputs
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
}

void loop()
{
  if (readDistance(sensor1) == true)
  {
    if (valid_data == true) {
      Serial.print("\stopLimit["); //These Serial.print lines remain for testing and adjustment purposes
      Serial.print(stopLimit);
      Serial.print("\tDist[");
      Serial.print(distance);
      Serial.println();

      if (distance <= stopLimit) {
        digitalWrite(redLED, HIGH);
        digitalWrite(yellowLED, LOW);
        digitalWrite(greenLED, LOW);
      }
      else if (distance > stopLimit && distance < (stopLimit + 200) ) { //change this number to increase distance yellow stays lit
        digitalWrite(redLED, LOW);
        digitalWrite(yellowLED, HIGH);
        digitalWrite(greenLED, LOW);
      }
      else if (distance > (stopLimit + 199) ) { //change this number to adjust when yellow LED illuminates
        digitalWrite(redLED, LOW);
        digitalWrite(yellowLED, LOW);
        digitalWrite(greenLED, HIGH);
      }
    }

    //  else {
    //   Serial.println("Read fail");
    //  }

    delay(50); //Delay small amount between readings
  }
}


//Write two bytes to a spot
boolean readDistance(uint8_t deviceAddress)
{
  Wire.beginTransmission(deviceAddress);
  Wire.write(0x01); //MSB
  Wire.write(0x02); //LSB
  Wire.write(7); //Data length: 7 bytes for distance data
  if (Wire.endTransmission(false) != 0) {
    return (false); //Sensor did not ACK
  }
  Wire.requestFrom(deviceAddress, (uint8_t)7); //Ask for 7 bytes

  if (Wire.available())
  {
    for (uint8_t x = 0 ; x < 7 ; x++)
    {
      uint8_t incoming = Wire.read();

      if (x == 0)
      {
        //Trigger done
        if (incoming == 0x00)
        {
          //Serial.print("Data not valid: ");//for debugging
          valid_data = false;
          //return(false);
        }
        else if (incoming == 0x01)
        {
          Serial.print("Data valid:     ");
          valid_data = true;
        }
      }
      else if (x == 2)
        distance = incoming; //LSB of the distance value "Dist_L"
      else if (x == 3)
        distance |= incoming << 8; //MSB of the distance value "Dist_H"
      else if (x == 4)
        strength = incoming; //LSB of signal strength value
      else if (x == 5)
        strength |= incoming << 8; //MSB of signal strength value
      else if (x == 6)
        rangeType = incoming; //range scale
    }
  }
  else
  {
    Serial.println("No wire data avail");
    return (false);
  }

  return (true);
}

//That's the end
