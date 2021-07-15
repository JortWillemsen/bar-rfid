#pragma once



namespace barTap {

  

  class rg{
    private:
      // define pins of the rgb led(s)
      int ledPinBlue = -1;
      int ledPinGreen = 15;
      int ledPinRed = 1;

    public:
      rg(){
        //Set pinmode of the rgb pins
        pinMode(ledPinRed, OUTPUT);
        pinMode(ledPinGreen, OUTPUT);
        setOff();
      }

      void setColor(const int& red, const int& green){
        if(red > 255 || red < 0 || green > 255 || green < 0){
          return;
        }
        analogWrite(ledPinRed, red);
        analogWrite(ledPinGreen, green);
      
      }

      void setRed(){
        setColor(255,0);
      }

      void setGreen(){
        setColor(0,255);
      }

      void setOrange(){
        setColor(230,255);
      }

      void setOff(){
        setColor(0,0);
      }
      
  };
}