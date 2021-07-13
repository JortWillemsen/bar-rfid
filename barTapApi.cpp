#include "barTapApi.h"

//      api
bool barTap::api::startWifi(){
  Serial.println("startWifi");

  //counter
  int counter0 = 0;

  //connect wifi 
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED && counter0 <10) {
    counter0++;

    delay(1000);
    Serial.println("Connecting..");

  }
  if(counter0 >= 10){
    Serial.println("startWifi failed after 10x");
    return false;
  }return true;
}

bool barTap::api::login(){
  StaticJsonDocument<64> doc; //64 bytes
  doc["userIdentification"] = UserID;
  doc["password"] = UserPassword;

  String JsonString = "";
  serializeJson(doc, JsonString);//Creates Json String for Authentication


  httpClient.begin(ApiUrl + "authenticate");  //Specify request destination

  httpClient.addHeader("Content-Type", "application/json");
  httpClient.addHeader("Accept", "*/*");

  httpClient.collectHeaders(headerKeys, numberOfHeaders);

  int httpCode = httpClient.POST(JsonString);

  bool Correct = true;

  if(httpCode >= 200 && httpCode <= 299){
    access_token = httpClient.header("access_token");
    refresh_token = httpClient.header("refresh_token");  
  }else{
    Correct = false;
    Serial.println("Authentication failed: ");
    Serial.println(httpCode);
  }

  httpClient.end(); //close http

  return Correct;
}

bool barTap::api::refresh_login(){
  Serial.println("refreshing login!");

  StaticJsonDocument<512> doc; //512 bytes
  doc["accessToken"] = access_token;
  doc["refreshToken"] = refresh_token;

  String JsonString = "";
  serializeJson(doc, JsonString);//Creates Json String for Authentication

  //serializeJsonPretty(doc, Serial);

  httpClient.begin(ApiUrl + "authenticate/refresh");  //Specify request destination

  addHeaders();

  httpClient.collectHeaders(headerKeys, numberOfHeaders);

  int httpCode = httpClient.POST(JsonString);

  bool Correct = true;

  if(httpCode >= 200 && httpCode <= 299){
    access_token = httpClient.header("access_token");
    refresh_token = httpClient.header("refresh_token");  
  }else{
    Correct = false;
    Serial.println("Authentication refresh failed: ");
    Serial.println(httpCode);
  }

  httpClient.end(); //close http

  return Correct;
}

void barTap::api::addHeaders(){
  httpClient.addHeader("Content-Type", "application/json");
  httpClient.addHeader("Accept", "*/*");
  httpClient.addHeader("token_type", "bearer");
  httpClient.addHeader("access_token", access_token);
}

barTap::api::api(HTTPClient& httpClient_, barTap::display* display_):
  httpClient(httpClient_),
  display(display_)
{
  //display set connecting
  if(display != nullptr){
    display->draw_connecting();
  }

  //Check wifi connection
  if(WiFi.status() == WL_CONNECTED){
    wifi_connection = true;
  }else{
    wifi_connection = startWifi();
    if(!wifi_connection){
      return;
    }
  }

  //If wifi is connected get token
  if(wifi_connection){
    logged_in = login();
    if(logged_in){
      Serial.println("got token");
    }
  }
  //refresh_login();
}

barTap::session_return barTap::api::getSession(const int& BarID, bool retry){
  if(!logged_in){
    return session_return(0,"", false, 0);
  }

  httpClient.begin(ApiUrl + "bars/" + String(BarID) + "/sessions/active");

  addHeaders();

  int httpCode = httpClient.GET();

  if(httpCode >= 200 && httpCode <= 299){
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, httpClient.getString());
    return session_return(doc["id"], doc["name"], true, httpCode);
  }
  else if(httpCode == 403){
    if(retry){
      httpClient.end(); //close http
      return session_return(0,"403", false, httpCode);
    }
    refresh_login();
    Serial.println("403 retrying!");
    httpClient.end(); //close http
    return getSession(BarID, true);
  }
  return session_return(0,"", false, httpCode);
}

String barTap::api::getBarName(const int& BarID, bool retry){ //get the barname
  if(!logged_in){
    return "";
  }

  httpClient.begin(ApiUrl + "bars/" + String(BarID));

  addHeaders();

  int httpCode = httpClient.GET();

  if(httpCode >= 200 && httpCode <= 299){
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, httpClient.getString());
    httpClient.end(); //close http
    return doc["name"];
  }
  else if(httpCode == 403){
    if(retry){
      
      httpClient.end(); //close http
      return "";
    }
    refresh_login();
    Serial.println("403 retrying!");
    httpClient.end(); //close http
    return getBarName(BarID, true);
  }
  httpClient.end(); //close http
  return "";
}

int barTap::api::addPersonToSession(const int& sessionID, const int& personID, const int& BarID, bool retry){ //return httpCode
  if(!logged_in){
    return 0;
  }

  StaticJsonDocument<64> doc0; //64 bytes
  doc0["customerId"] = personID;

  String JsonString = "";
  serializeJson(doc0, JsonString);//Creates Json String for Authentication
  //serializeJsonPretty(doc0, Serial);

  httpClient.begin(ApiUrl + "bars/" + String(BarID) + "/sessions/" + String(sessionID)+ "/");

  addHeaders();

  int httpCode = httpClient.POST(JsonString);

  if(httpCode == 403){
    if(retry){
      
      httpClient.end(); //close http
      return httpCode;
    }
    refresh_login();
    Serial.println("403 retrying!");
    httpClient.end(); //close http
    return addPersonToSession(sessionID, personID, BarID, true);
  }

  

  httpClient.end(); //close http

  return httpCode;
}

String barTap::api::getMoneyDue(const int& sessionID, const int& personID, const int& BarID, bool retry){ // return the string with the money to pay from the personID
  if(logged_in){
    httpClient.begin(ApiUrl + "bars/" + String(BarID) + "/sessions/" + String(sessionID) + "/bills");

    addHeaders();

    int httpCode = httpClient.GET();

    if(httpCode >= 200 && httpCode <= 299){
      DynamicJsonDocument doc(1024);
      //DynamicJsonDocument docSerial(1024);

      StaticJsonDocument<200> filter; //get only customer id and totalPrice
      filter[0]["customer"]["id"] = true;
      filter[0]["totalPrice"] = true;
      filter[0]["payed"] = true;
      
      deserializeJson(doc, httpClient.getString(), DeserializationOption::Filter(filter));

      //deserializeJson(docSerial, httpClient.getString());
      //serializeJsonPretty(doc, Serial);

      JsonArray arr = doc.as<JsonArray>(); //make array for for loop

      int toPay = 0;

      //loop through items
      for(JsonVariant value : arr){
        if(value["customer"]["id"] == personID){
          if(value["payed"] == true){
            httpClient.end(); //close http
            return "payed";
          }
          httpClient.end(); //close http
          return value["totalPrice"].as<String>();
        }
      }

      httpClient.end(); //close http
      return "";
    }else if(httpCode == 403){
      if(!retry){
        refresh_login();
        Serial.println("403 retrying!");
        httpClient.end(); //close http
        return getMoneyDue(sessionID, personID, BarID, true);
      }//else:
      httpClient.end(); //close http
      return "";
    }
  }
  httpClient.end(); //close http
  return "";
}

barTap::firstAndLastName barTap::api::getName(const int& personID, const int& BarID, bool retry){
  if(logged_in){
    httpClient.begin(ApiUrl + "bars/" + String(BarID) + "/people/" + String(personID));

    addHeaders();

    int httpCode = httpClient.GET();

    if(httpCode >= 200 && httpCode <= 299){
      DynamicJsonDocument doc(384);

      deserializeJson(doc, httpClient.getString());

      //serializeJsonPretty(doc, Serial);

      String name = doc["name"];
      int index_space = name.indexOf(' ');

      httpClient.end(); //close http
      if(index_space >= 0){
        return firstAndLastName(name.substring(0,index_space), name.substring(index_space+1, name.length()));
      }//else if no space in name
      return firstAndLastName("", name);

    }else if(httpCode == 403){
      if(!retry){
        refresh_login();
        Serial.println("403 retrying!");
        httpClient.end(); //close http
        return getName(personID, BarID, true);
      }//else:
      httpClient.end(); //close http
      return firstAndLastName("error", "");
    }
  }
  httpClient.end(); //close http
  return firstAndLastName("error", "");
}



//      info
bool barTap::info::updateSession(){
  auto r = barAPI.getSession(BarID);
  if(r.done){
    SessionID = r.id;
    SessionName = r.name;
    sessionActive = true;
    return true;
  }else if(r.httpCode == 404){
    SessionName = "No Active Session";
    sessionActive = false;
  }
  return false;
}

bool barTap::info::updateBarName(){
  String name = barAPI.getBarName(BarID);
  if( name != ""){
    BarName = name;
    return true;
  }
  return false;
}

barTap::info::info(api& barAPI_):
  barAPI(barAPI_)
{
  updateAll();
}

int barTap::info::addPersonToActiveSession(const int& personID){
  if(sessionActive){
    int code = barAPI.addPersonToSession(SessionID, personID, BarID);
    if(code >= 200 && code <= 299){
      return 1;
    }else if(code == 409){
      return 2;
    }
  }
  return 0;
}

String barTap::info::getMoneyToPay(const int& personID){
  return barAPI.getMoneyDue(SessionID, personID, BarID);
}

bool barTap::info::updateAll(){
  return updateSession() && updateBarName();
}

int barTap::info::getSessionID(){
  updateSession();
  return SessionID;
}

String barTap::info::getSessionName(){
  updateSession();
  return SessionName;
}

String barTap::info::getBarName(){
  updateBarName();
  return BarName;
}
