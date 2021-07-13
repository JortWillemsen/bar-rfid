#include "barTap.h"

void barTap::display::draw_idle_screen(String barName, String sessionName){
  //check string length
  if(barName.length() > 21){
    barName = barName.substring(0,19);
    barName += "..";
  }

  if(sessionName.length() > 21){
    sessionName = sessionName.substring(0,19);
    sessionName += "..";
  }
  
  //clear display ahead of new items
  oled.clearDisplay();

  //make filled rectangle for white welkom background
  oled.fillRect(0,0,128,20,SSD1306_WHITE);

  //first inverse welkom text on 1x scale
  oled.setCursor(1,3);
  oled.setTextSize(2);
  oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  oled.println("  BARTAP");

  //sessionName text on 1x scale
  oled.setCursor(1,23);
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.println(barName);

  //extra instruction text on 1x scale
  oled.setCursor(1,52);
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.println(sessionName);

  //display items on display
  oled.display();
}

void barTap::display::draw_name(String firstName, const String& lastName, const uint16_t& personID){
  if(firstName.length() > 10){
    firstName = firstName.substring(0,8);
    firstName += "..";
  }
  
  //clear display ahead of new items
  oled.clearDisplay();

  //make filled rectangle for white welkom background
  oled.fillRect(0,0,128,20,SSD1306_WHITE);

  //first inverse welkom text on 1x scale
  oled.setCursor(1,3);
  oled.setTextSize(2);
  oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  oled.println(F("Welkom"));

  //first name text on 2x scale
  oled.setCursor(1,23);
  oled.setTextSize(2);
  oled.setTextColor(SSD1306_WHITE);
  oled.println(firstName);

  //last name text on 1x scale
  oled.setCursor(1,40);
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.println(lastName);

  //last name text on 1x scale
  oled.setCursor(1,55);
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.println(personID);

  //display items on display
  oled.display();
}

//draw a screen with the word "not valid" on top and "!!!" in the middle
void barTap::display::draw_not_valid(){  
  //clear display ahead of new items
  oled.clearDisplay();

  //make filled rectangle for white welkom background
  oled.fillRect(0,0,128,20,SSD1306_WHITE);

  //first inverse welkom text on 1x scale
  oled.setCursor(1,3);
  oled.setTextSize(2);
  oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  oled.println(F(" Not Valid"));

  //'!!!' on screen
  oled.setCursor(32,22);
  oled.setTextSize(4);
  oled.setTextColor(SSD1306_WHITE);
  oled.println(F("!!!"));
 
  //display items on display
  oled.display();
}

void barTap::display::draw_error(String error){
  //clear display ahead of new items
  oled.clearDisplay();

  //make filled rectangle for white welkom background
  oled.fillRect(0,0,128,20,SSD1306_WHITE);

  //first inverse Error text on 1x scale
  oled.setCursor(1,3);
  oled.setTextSize(2);
  oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  oled.println(F("Error"));
  
  //display error

  oled.setCursor(1,23);
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.println(error);
 
  //display items on display
  oled.display();
}

void barTap::display::draw_connecting(const String& input){
  //clear display ahead of new items
  oled.clearDisplay();

  //make filled rectangle for white welkom background
  oled.fillRect(0,0,128,20,SSD1306_WHITE);

  //first inverse Wifi text on 1x scale
  oled.setCursor(32,3);
  oled.setTextSize(2);
  oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  oled.println(F("WIFI"));

  //display "Connecting" or input
  oled.setCursor(1,23);
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  if(input != ""){
    oled.println(input);
  }else{
    oled.println("CONNECTING");
  }
  
  //display items on display
  oled.display();
}

void barTap::display::draw_moneyToPay(const String& name, const String& amount){
  //clear display ahead of new items
  oled.clearDisplay();

  //make filled rectangle for white welkom background
  oled.fillRect(0,0,128,20,SSD1306_WHITE);

  //first inverse Wifi text on 1x scale
  oled.setCursor(1,3);
  oled.setTextSize(2);
  oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  oled.println(F("REKENING"));

  //display "Connecting" or input
  oled.setCursor(1,23);
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.println(name);

  //extra instruction text on 1x scale
  oled.setCursor(1,52);
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  if(amount == "payed"){
    oled.println("Betaald");
  }else{
    oled.println(String(amount) + " euro");
  }

  //display items on display
  oled.display();
}