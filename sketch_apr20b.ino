#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <string>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// LEDS
#define S0 5

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

struct user_data{
    String firstName;
    uint64_t id;
    String lastName;
    String mail;
    String username;
};

struct person_data{
    int id;
    user_data userData;

    person_data() : id(0), userData(user_data{
                                            "firstName",
                                            0,
                                            "lastname",
                                            "mail",
                                            "username"
                                    }
            ) {}

    person_data(int id, const user_data &userData) : id(id), userData(userData) {}
};

class RFID_Storage{
private:
    person_data person_data_storage[255];
    int index = 0;

public:
    bool add(const person_data& id){
        if(index <= 255) {
            person_data_storage[index] = id;
            index++;
            return true;
        }
        return false;
    }

    signed int locate_id(const uint64_t & id){
        for(int i = 0; i < index; i++){
            if(person_data_storage[i].userData.id == id){
                return i;
            }
        }
        return -1;
    }

    person_data get_from_id(const uint64_t& id){
        signed int location = locate_id(id);
        if(location >= 0) {
            return person_data_storage[location];
        }return person_data();
    }
};

class RFID_Reader{ //TEMP
public:
    uint64_t tag0 = 0x4464302869dcf56b; //Daan  0d8bc15368733b14
    uint64_t tag1 = 0x0e6c4cc2844336bb; //      4783bdb985ecf8c4
    uint64_t tag2 = 0xde0645963c332d19; //      97e9b4ed3d9ce366
    uint64_t tag3 = 0x9fe5f6f1cb01600d; //      d60a078acaaeae72

    uint64_t tag4 = 0x656b73c2a9bcdd21;
    uint64_t tag5 = 0x9d765f78dd95daa7;
    uint64_t tag6 = 0x79ab9d6b99ad5c21;
    uint64_t tag7 = 0x44fc69f86c9d04c3;

    uint64_t read(const bool& known){
        if(known){
            return tag0;
        }else{
            return tag4;
        }
    }
};

class Server_temp{
    //currently not not using the api just predefined numbers.
private:
    RFID_Storage& rfidStorage;

    person_data personData0{
            0,
            user_data{
                "Daan",
                0x0d8bc15368733b14,
                "Kerbusch",
                "daankerbusch@gmail.com",
                "Strijderdaan"
            }
    };

    person_data personData1{
            0,
            user_data{
                    "Apple",
                    0x4783bdb985ecf8c4,
                    "lastApple",
                    "Apple@gmail.com",
                    "AppleBest"
            }
    };

    person_data personData2{
            0,
            user_data{
                    "Pear",
                    0x97e9b4ed3d9ce366,
                    "lastPear",
                    "Pear@gmail.com",
                    "PearBest"
            }
    };

    person_data personData3{
            0,
            user_data{
                    "Orange",
                    0xd60a078acaaeae72,
                    "lastOrange",
                    "Orange@gmail.com",
                    "OrangeBest"
            }
    };


public:
    Server_temp(RFID_Storage &rfidStorage) : rfidStorage(rfidStorage) {
        rfidStorage.add(personData0);
        rfidStorage.add(personData1);
        rfidStorage.add(personData2);
        rfidStorage.add(personData3);
    }
};

class encryption64{
private:
    uint64_t key;

public:
    encryption64(uint64_t key) : key(key) {}

    uint64_t encrypt_id(const uint64_t& id){
        return id^key;
    }

    uint64_t decrypt_id(const uint64_t& id){
        return key^id;
    }
};

class RFID_Control{
private:
    RFID_Reader& rfidReader;
    Server_temp& server;
    RFID_Storage& rfidStorage;

    const uint64_t encryption_key;

public:
    RFID_Control(
            RFID_Reader &rfidReader,
            Server_temp &server,
            RFID_Storage &rfidStorage,
            const uint64_t encryptionKey) :
        rfidReader(rfidReader),
        server(server),
        rfidStorage(rfidStorage),
        encryption_key(encryptionKey)
    {}

    void run(){
        uint64_t input = rfidReader.read(true);
        input = encryption64(encryption_key).decrypt_id(input);
        if(rfidStorage.locate_id(input) >= 0){
            person_data personData = rfidStorage.get_from_id(input);
            Serial.println(personData.userData.firstName);
            Serial.println(personData.userData.lastName);
            
        }else{
            Serial.println(F("unknown id"));
        }
    }
};



void setup() {
  //pin init
  pinMode(S0, OUTPUT);
  
  Serial.begin(9600);

  //SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  RFID_Reader reader;

  RFID_Storage storage;

  Server_temp server(storage);

  uint64_t key = 0x49eff17b01afce7f;

  RFID_Control rfidControl(reader, server, storage, key);

  digitalWrite(S0,HIGH);

  rfidControl.run();

  while(true){
    draw_name(storage.get_from_id(0xd60a078acaaeae72).userData);
    delay(2000);
    draw_idle_screen("Bartjes BAR", "Oudjaarsavond");
    delay(2000);
  }
}

void loop() {
  
}

void draw_name(const user_data& userData){
  String firstName = userData.firstName;
  
  if(firstName.length() > 10){
    firstName = firstName.substring(0,8);
    firstName += "..";
  }
  
  //convert uint64_t to int;
  unsigned int id = userData.id;
  
  //clear display ahead of new items
  display.clearDisplay();

  //make filled rectangle for white welkom background
  display.fillRect(0,0,128,20,SSD1306_WHITE);

  //first inverse welkom text on 1x scale
  display.setCursor(1,3);
  display.setTextSize(2);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.println(F("Welkom"));

  //first name text on 2x scale
  display.setCursor(1,23);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.println(firstName);

  //last name text on 1x scale
  display.setCursor(1,40);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println(userData.lastName);

  //last name text on 1x scale
  display.setCursor(1,55);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println(id, HEX);

  //display items on display
  display.display();
}

void draw_idle_screen(String barName, const String& sessionName){
  //check string length
  if(barName.length() > 10){
    barName = barName.substring(0,8);
    barName += "..";
  }
  
  //clear display ahead of new items
  display.clearDisplay();

  //make filled rectangle for white welkom background
  display.fillRect(0,0,128,20,SSD1306_WHITE);

  //first inverse welkom text on 1x scale
  display.setCursor(1,3);
  display.setTextSize(2);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.println(barName);

  //last name text on 1x scale
  display.setCursor(1,23);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println(sessionName);

  //last name text on 1x scale
  display.setCursor(1,46);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println(F("Open de app en houd\nhet tegen het scherm!"));

  //display items on display
  display.display();
}
