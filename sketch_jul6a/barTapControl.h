#pragma once

#include "barTap.h"
#include "barTapApi.h"
#include "barTapNfc.h"
#include "encryption32.h"

//rgb
#include "barTapRgb.h"


namespace barTap{
  class control{
    private:
      barTap::display display;

      HTTPClient http;
      barTap::api api = barTap::api(http, &display);
      barTap::info barInfo = (api);

      barTap::nfc nfcReader;

      barTap::rg rg;

      uint_fast32_t key;
      barTap::encryption32 encryption = (key);

      void displayDefaultScreen(){
        display.draw_idle_screen(barInfo.getBarName(), barInfo.getSessionName());
      }

      void displayAddedToSessionScreen(const uint16_t& personID){
        barTap::firstAndLastName name = barInfo.getNameAsStruct(personID);
        if(name.firstName == "error"){
          display.draw_error("Unknown ID");
          delay(100);
          rg.setRed();
          return;
        }
        display.draw_name(name.firstName, name.lastName, personID);
      }

      void displayMoneyToPay(const uint16_t& personID){
        barTap::firstAndLastName name = barInfo.getNameAsStruct(personID);
        if(name.firstName == "error"){
          
          display.draw_error("Unknown ID");
          delay(100);
          rg.setRed();
          return;
        }
        String nameFull = name.firstName;
        nameFull += " ";
        nameFull += name.lastName;
        display.draw_moneyToPay(nameFull, barInfo.getMoneyToPay(personID));
      }

      void handlePersonID(const int& personID){
        if(barInfo.addPersonToActiveSession(personID) == 1){
          //add person to active session
          displayAddedToSessionScreen(personID);
          rg.setGreen();
          delay(2000);
          rg.setOff();
          displayDefaultScreen();
          return;
        }//else show amount to pay
        displayMoneyToPay(personID);
        rg.setOrange();
        delay(2000);
        rg.setOff();
        displayDefaultScreen();
        return;
      }

    public:
      control(const uint_fast32_t& key_):
        key(key_)
      {
        displayDefaultScreen();
      }

      void run(){
        while(true){
          yield();
          int r = nfcReader.read();
          if(r >=0){
            r = encryption.decrypt(r);
            handlePersonID(r);
          }
        }
      }
  };
}