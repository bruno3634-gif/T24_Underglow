#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ESP32_CAN.h>

#define NUM_LEDS 48
#define PIN 16

Adafruit_NeoPixel underglow = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
TWAI_Interface CAN(1000, 21, 22);

int ID = 0;
volatile int flag_prev_value = 0;
volatile int msg = 0;
volatile int mode = 0;
unsigned long timeout = 0;

void mode_selector();
void underglow_const();
void underglow_breathing();
void underglow_fluerscent_lamp();
void o_ponte_e_paneleiro();

void setup()
{
  underglow.begin();
  underglow.clear();
  underglow.show();
}

void loop()
{
  ID = CAN.RXpacketBegin();
  if (ID = 0x191)
  {
    CAN.RXgetDLC();
    msg = CAN.RXpacketRead(0);
    if (flag_prev_value == 0 && msg == 5)
    {
      mode++;
      flag_prev_value = 1;
    }
    else
    {
      if (flag_prev_value == 1 && msg == 1)
      {
        flag_prev_value = 0;
      }
    }

    if (mode > 3)
    {
      mode = 0;
    }

    if (msg == 3)
    {
      underglow.fill(underglow.Color(250, 236, 55), 0, underglow.numPixels());
      mode_selector();
    }
    else
    {
      underglow.fill(underglow.Color(0, 0, 0), 0, underglow.numPixels());
      underglow.show();
    }
  }
}

void mode_selector()
{
  switch (mode)
  {
  case 0:
    underglow_const();
    break;
  case 1:
    underglow_breathing();
    break;
  case 2:
    underglow_fluerscent_lamp();
    break;
  case 3:
    o_ponte_e_paneleiro();
    break;
  default:
    underglow.fill(underglow.Color(0, 0, 0), 0, underglow.numPixels());
    underglow.show();
    while (1)
    {
      if (CAN.RXpacketBegin() == 0x191 && CAN.RXpacketRead(0) == 1)
      {
        break;
      }
    }
    break;
  }
}

void underglow_const()
{
  while (1)
  {
    underglow.fill(underglow.Color(250, 236, 10), 0, underglow.numPixels());
    underglow.show();
    delay(200);
    if (CAN.RXpacketBegin() == 0x191 && CAN.RXpacketRead(0) == 1)
    {
      break;
    }
  }
}

void underglow_breathing()
{
  while (CAN.RXpacketBegin() != 0x191 && CAN.RXpacketRead(0) != 1)
  {
    for (int i = 0; i <= 255; i++)
    {
      underglow.fill(underglow.Color(236, 236, 10), 0, underglow.numPixels());
      underglow.setBrightness(i);
      underglow.show();
      while (millis() < timeout + 4)
        ;
      timeout = millis();
    }
    for (int i = 255; i >= 0; i--)
    {
      underglow.fill(underglow.Color(250, 236, 10), 0, underglow.numPixels());
      underglow.setBrightness(i);
      underglow.show();
      while (millis() < timeout + 6)
        ;
      timeout = millis();
    }

  }
}

void underglow_fluerscent_lamp()
{
  underglow.fill(underglow.Color(250, 236, 10), 0, underglow.numPixels());
  underglow.setBrightness(255);
  underglow.show();
  timeout = millis();
  while (millis() < timeout + 300)
    ;
  underglow.fill(underglow.Color(0, 0, 0), 0, underglow.numPixels());
  underglow.show();
  timeout = millis();
  while (millis() < timeout + 500)
    ;
  underglow.fill(underglow.Color(250, 236, 10), 0, underglow.numPixels());
  underglow.setBrightness(100);
  underglow.show();
  timeout = millis();
  while (millis() < timeout + 300)
    ;
  underglow.fill(underglow.Color(0, 0, 0), 0, underglow.numPixels());
  underglow.show();
  timeout = millis();
  while (millis() < timeout + 800)
    ;
  while (1)
  {
    underglow.fill(underglow.Color(250, 236, 10), 0, underglow.numPixels());
    underglow.setBrightness(255);
    underglow.show();
    if (CAN.RXpacketBegin() == 0x191 && CAN.RXpacketRead(0) == 1)
    {
      break;
    }
    delay(300);
  }
}

void o_ponte_e_paneleiro()
{
  while (CAN.RXpacketBegin() != 0x191 && CAN.RXpacketRead(0) != 1)
  {
    for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256)
    {
      for (int i = 0; i < underglow.numPixels(); i++)
      {
        // Map each pixel to a different part of the color wheel
        int pixelHue = firstPixelHue + (i * 65536L / underglow.numPixels());
        underglow.setPixelColor(i, underglow.gamma32(underglow.ColorHSV(pixelHue)));
        delay(1);
      }
      underglow.show();
    }
   
  }
}