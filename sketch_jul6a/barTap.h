#pragma once

//include oled libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//OLED defines
#define SCREEN_WIDTH    128 // OLED display width, in pixels
#define SCREEN_HEIGHT   64  // OLED display height, in pixels
#define OLED_RESET      D0  // Reset pin # (or -1 if sharing Arduino reset pin)

namespace barTap{

  class display{
    private:
      Adafruit_SSD1306 oled = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

    public:
      display(){
        if(!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
          //Serial.println(F("SSD1306 allocation failed"));
          for(;;); // Don't proceed, loop forever
        }
      }
      Adafruit_SSD1306& getOled(){
        return oled;
      }

      //draw a screen with the barName on top
      void draw_idle_screen(String barName, String sessionName);

      //draw a screen with the Welkom on top with Firstname, lastname and id below
      void draw_name(String firstName, const String& lastName, const uint16_t& personID);

      //draw a screen with the word "not valid" on top and "!!!" in the middle
      void draw_not_valid();

      //draw a screen with the word "error" on top and the error below
      void draw_error(String error);

      //draw "wifi" on top and input if given otherwise "connecting"
      void draw_connecting(const String& input = "");

      void draw_moneyToPay(const String& name, const String& amount);
  };
}
