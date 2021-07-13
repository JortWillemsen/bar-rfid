#include "barTapControl.h"

//#include "barTap.h"
//#include "barTapApi.h"
//#include "barTapNfc.h"
//#include "encryption32.h"

//#include "barTapRgb.h"


//#include <ESP8266WiFi.h>
//#include <ESP8266HTTPClient.h>

//barTap::encryption32 encryption(0x14D2405C);

void setup() {
  Serial.begin(115200);
  Serial.println();

  barTap::control control(0x14D2405C);
  //control.displayAddedToSessionScreen(1734);
  //delay(1000);
  //control.displayAddedToSessionScreen(3);
  //delay(1000);
  //control.displayAddedToSessionScreen(1);
  control.run();

  //barTap::rgb rgb;
  //rgb.setColor(255,255,255);

  //barTap::display display;
  //display.draw_name("Daan", "Kerbusch", 1734);

  // put your setup code here, to run once:
  //barTap::rfid_dummy rfid;
  //rfid.read(false);
  //Serial.println(rfid.read(false));
  //Serial.println(rfid.read(true));

  
  /*barTap::display d;
  d.draw_connecting();*/

  /*delay(1000);
  d.draw_name("Daan", "Kerbusch", 1734);
  delay(1000);
  d.draw_not_valid();
  delay(1000);
  d.draw_error("Robot uprising");

  barTap::encryption32 encryption(0x14D2405C);
  Serial.println(encryption.encrypt(1734));
  Serial.println(encryption.decrypt(349324954));*/

  /*HTTPClient http2;
  auto bar_api = barTap::api(http2);
  barTap::info bar_info = (bar_api);
  bar_info.printName();*/

  /*d.draw_idle_screen(bar_info.getBarName(), bar_info.getSessionName());*/

  //Serial.println(bar_info.getSessionID());
  //Serial.println(bar_info.getSessionName());
  //Serial.println(bar_info.getBarName());
  
  //Serial.println();
  //Serial.println(bar_info.addPersonToActiveSession(1734));
  //Serial.println();

  //Serial.println(bar_info.getMoneyToPay(1734));

  /*barTap::nfc nfcReader;
  nfcReader.getInfo();

  int r = 0;

  while(true){
    yield();
    r = nfcReader.read();
    if(r >=0){
      r = encryption.decrypt(r);
      Serial.println(r);
      if(bar_info.addPersonToActiveSession(r) == 2){
        Serial.println(bar_info.getMoneyToPay(r));
      }
    }
  }*/

}

void loop() {
  yield();
  //Serial.println("loop");
  delay(1000);
}
