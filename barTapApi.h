//include wifi and json
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

//include display:
#include "barTap.h"

//API WIKI = http://tungstun-bar-api.herokuapp.com/swagger-ui/index.html

#define STASSID "FRITZ!Box 5490 SW"
  #define STAPSK  "93226223717081710843"
//


namespace barTap{
  struct session_return{
    int id;
    String name;
    bool done;
    int httpCode;
    session_return(const int& id_, const String& name_, const bool& done_, const int& httpCode_):
      id(id_), name(name_), done(done_), httpCode(httpCode_)
    {}
  };

  struct firstAndLastName{
    String firstName;
    String lastName;

    firstAndLastName(const String& firstName_, const String& lastName_):
      firstName(firstName_),
      lastName(lastName_)
    {}
  };

  class api{
    private:
      HTTPClient& httpClient;

      const char* ssid = STASSID;
      const char* password = STAPSK;

      bool logged_in = false;
      String access_token = "";
      String refresh_token = "";

      String ApiUrl = "http://tungstun-bar-api.herokuapp.com/api/";
      String UserID = "admin";
      String UserPassword = "password";

      //Headers for authentication
      const char * headerKeys[2] = {"access_token", "refresh_token"};
      const size_t numberOfHeaders = 2;

      //display used for saying connected if given
      barTap::display* display;

      bool startWifi();

      bool login();
      bool refresh_login();

      void addHeaders();
    
    public:
      //public members
      bool wifi_connection = false;

      //constructor
      api(HTTPClient& httpClient_, barTap::display* display_ = nullptr);

      //public functions
      session_return getSession(const int& BarID, bool retry = false);
      String getBarName(const int& BarID, bool retry = false);
      int addPersonToSession(const int& sessionID, const int& personID, const int& BarID, bool retry = false);

      // getMoneyDue = URL/bars/BarID/sessions/SessionID/bills
      String getMoneyDue(const int& sessionID, const int& personID, const int& BarID, bool retry = false);

      firstAndLastName getName(const int& personID, const int& BarID, bool retry = false);
  };

  class info{
    private:
      api& barAPI;

      int BarID = 1;
      String BarName = "De Debug Bar";

      int SessionID = -1;
      String SessionName = "";
      bool sessionActive = false;

      bool updateSession();
      bool updateBarName();

    public:
      info(api& barAPI_);

      int addPersonToActiveSession(const int& personID);

      String getMoneyToPay(const int& personID);

      firstAndLastName getNameAsStruct(const int& personID){
        return barAPI.getName(personID, BarID);
      }

      bool updateAll();

      int getSessionID();
      String getSessionName();
      String getBarName();
  };
}