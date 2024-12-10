#include <Wire.h>
#include <SevSeg.h>

SevSeg sevseg;
int potValue;
int cooldown = 0;

void setup() {
  Wire.begin();
  Serial.begin(9600);

  byte numDigits = 4;
  byte digitPins[] = {2, 3, 4, 5};
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};
  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins, 0);
  sevseg.setBrightness(90);
}

void loop() {
  //Radio mode
  if (analogRead(0) == 0)
    while (true)
    {
      setFrequency(((map(analogRead(0), 0, 1023, 950, 1080)) * 0.1));
      sevseg.setNumber((map(analogRead(0), 0, 1023, 950, 1080)), 1);
      for (int i = 0; i < 100; i++)
      {      
        sevseg.refreshDisplay();
        delay(1);
      }
    }

  //Spirit box mode
  int time = 20;
  
  for (float freq = 95.0; freq <= 108.1; freq += 0.1)
  {
    if (analogRead(0) == 0)
      freq -= 0.1;
    
    setFrequency(freq - 0.1);
    sevseg.setNumber(freq * 10, 1);

    for (int i = 0; i < time * 10; i++)
    {
      time = map(analogRead(0), 0, 1023, 35, 5);
      
      if (cooldown > 0)
      {
        if (analogRead(0) == 0)
          sevseg.setNumber(0, -1);
        else
          sevseg.setNumber(time * 10, -1);
        cooldown--;
      }

      sevseg.refreshDisplay();
      potValue = analogRead(0);
      delay(1);
      if (analogRead(0) > potValue + 1 || analogRead(0) < potValue - 1)
        cooldown = 1000;
    }
  }
}

void setFrequency(double frequency) {
  int frequencyB = 4 * (frequency * 1000000 + 225000) / 32768;
  byte frequencyH = frequencyB >> 8;
  byte frequencyL = frequencyB & 0xFF;

  Wire.beginTransmission(0x60);
  Wire.write(frequencyH & 0b00111111);
  Wire.write(frequencyL);
  Wire.write(0b10110000);
  Wire.write(0b00010000);
  Wire.write(0b00000000);
  Wire.endTransmission();
}
