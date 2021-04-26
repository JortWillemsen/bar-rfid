//include i2c and spi pins
#include <SPI.h>
#include <Wire.h>

//include oled libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//RC522
#include <MFRC522.h>

//barName
String barName = "BartjesBAR";
//sesionName
String sesionName = "Oudjaarsavond";

//Init mfrc522 and its piuns
#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

//OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// define pins of the rgb led(s)
int ledPinBlue = 2;
int ledPinGreen = 3;
int ledPinRed = 4;

//buzzer pin
int buzzerPin = 6;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

struct user_data{
    String firstName;
    uint_fast32_t id;
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

//draw a screen with the Welkom on top with Firstname, lastname and id below
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

//draw a screen with the barName on top
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

  //sessionName text on 1x scale
  display.setCursor(1,23);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println(sessionName);

  //extra instruction text on 1x scale
  display.setCursor(1,46);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println(F("Open de app en houd\nhet tegen het scherm!"));

  //display items on display
  display.display();
}

//draw a screen with the word "not valid" on top and "!!!" in the middle
void draw_not_valid(){  
  //clear display ahead of new items
  display.clearDisplay();

  //make filled rectangle for white welkom background
  display.fillRect(0,0,128,20,SSD1306_WHITE);

  //first inverse welkom text on 1x scale
  display.setCursor(1,3);
  display.setTextSize(2);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.println(F(" Not Valid"));

  //'!!!' on screen
  display.setCursor(32,22);
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);
  display.println(F("!!!"));
 
  //display items on display
  display.display();
}

//draw a screen with the word "error" on top and the error below
void draw_error(String error){
  //clear display ahead of new items
  display.clearDisplay();

  //make filled rectangle for white welkom background
  display.fillRect(0,0,128,20,SSD1306_WHITE);

  //first inverse welkom text on 1x scale
  display.setCursor(1,3);
  display.setTextSize(2);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.println(F("Error"));
  
  //display error

  display.setCursor(1,23);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println(error);
 
  //display items on display
  display.display();
}

//RFID storage for person_data
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

    signed int locate_id(const uint_fast32_t & id){
        for(int i = 0; i < index; i++){
            if(person_data_storage[i].userData.id == id){
                return i;
            }
        }
        return -1;
    }

    person_data get_from_id(const uint_fast32_t& id){
        signed int location = locate_id(id);
        if(location >= 0) {
            return person_data_storage[location];
        }return person_data();
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
                1,
                "Kerbusch",
                "daankerbusch@gmail.com",
                "Strijderdaan"
            }
    };

    person_data personData1{
            0,
            user_data{
                    "Apple",
                    2,
                    "lastApple",
                    "Apple@gmail.com",
                    "AppleBest"
            }
    };

    person_data personData2{
            0,
            user_data{
                    "Pear",
                    3,
                    "lastPear",
                    "Pear@gmail.com",
                    "PearBest"
            }
    };

    person_data personData3{
            0,
            user_data{
                    "Orange",
                    4,
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

//encryptor and decryptor for a uint_fast32_t integer and a uint_fast32_t key
class encryption32{
  private:
    uint_fast32_t key;

  public:
    encryption32(uint_fast32_t key) : key(key) {}

    uint_fast32_t encrypt(const uint_fast32_t& id){
        return id^key;
    }

    uint_fast32_t decrypt(const uint_fast32_t& id){
        return key^id;
    }
};

//Function for setting the color of the leds
void setRGB(const int& red, const int& green, const int& blue){
  if(red > 255 || red < 0 || green > 255 || green < 0 || blue > 255 | blue < 0){
    return;
  }
  analogWrite(ledPinRed, red);
  analogWrite(ledPinGreen, green);
  analogWrite(ledPinBlue, blue);
}

//class that gets called by nfc_reader when a id is found
//it checks if the id is known in the storage
class RFID_Control{
  private:
    Server_temp& server;
    RFID_Storage& rfidStorage;

    const uint_fast32_t encryption_key;

  public:
    RFID_Control(
        Server_temp &server,
        RFID_Storage &rfidStorage,
        const uint_fast32_t encryptionKey) :
      server(server),
      rfidStorage(rfidStorage),
      encryption_key(encryptionKey)
    {}

    void run(uint_fast32_t input){
      //uint_fast32_t input = rfidReader.read(true);
      //input = encryption32(encryption_key).decrypt(input);
      if(rfidStorage.locate_id(input) >= 0){
        person_data personData = rfidStorage.get_from_id(input);

        draw_name(personData.userData);
        
        //Serial.println(personData.userData.firstName);
        //Serial.println(personData.userData.lastName);

        setRGB(0,255,0);

        delay(2000);
        setRGB(0,0,0);
        draw_idle_screen(barName, sesionName);
          
      }else{
        draw_not_valid();
        
        setRGB(255,0,0);
        digitalWrite(buzzerPin, HIGH);
        Serial.println(F("unknown id"));
        delay(250);
        setRGB(0,0,0);
        digitalWrite(buzzerPin, LOW);
        delay(250);
        setRGB(255,0,0);
        digitalWrite(buzzerPin, HIGH);
        delay(250);
        setRGB(0,0,0);
        digitalWrite(buzzerPin, LOW);
        delay(1250);

        draw_idle_screen(barName, sesionName);
      }
    }

    /*void run(uint_fast32_t input){
      Serial.println("RFID_Control");
      Serial.println(input);
      Serial.println("");
    }*/
};

//class checks the mfrc522 for new cards and reads that card giving the decrypted id to rfid_control
class nfc_reader{
  private:
  MFRC522& mfrc522;
  RFID_Control& RFIDControl;

  uint32_t encryption_key;

  public:
  nfc_reader(MFRC522& mfrc522, RFID_Control& RFIDControl, uint32_t encryption_key): 
    mfrc522(mfrc522),
    RFIDControl(RFIDControl),
    encryption_key(encryption_key)
  {}

  void read(){
    //*parts of this funtion is from example code from mfrc522 libary*
    // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
    MFRC522::MIFARE_Key key;
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

    //some variables we need
    byte block;
    byte len;
    MFRC522::StatusCode status;

    //-------------------------------------------

    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      return;
    }

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      Serial.println(F("**! mfrc522.PICC_ReadCardSerial()**"));
      return;
    }

    Serial.println(F("**Card Detected:**"));

    //-------------------------------------------

    //mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

    //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex

    //-------------------------------------------


    byte buffer1[18];

    block = 4;
    len = 18;

    //------------------------------------------- READ encryted id
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Authentication failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));

      //draw error on screen
      draw_error(mfrc522.GetStatusCodeName(status));
      
      //blink led blue to indicate a error
      setRGB(0,100,255);
      delay(250);
      setRGB(0,0,0);

      //wait for the remaining 2 seconds
      delay(1750);

      //draw idle screen with barName and sesionName
      draw_idle_screen(barName, sesionName);

    	//reset mfrc522
      mfrc522.PCD_Reset();
      mfrc522.PCD_Init();

      return;
    }

    status = mfrc522.MIFARE_Read(block, buffer1, &len);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Reading failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));

      //draw error on screen
      draw_error(mfrc522.GetStatusCodeName(status));
      
      //blink led blue to indicate a error
      setRGB(0,100,255);
      delay(250);
      setRGB(0,0,0);

      //wait for the remaining 2 seconds
      delay(1750);

      //draw idle screen with barName and sesionName
      draw_idle_screen(barName, sesionName);

    	//reset mfrc522
      mfrc522.PCD_Reset();
      mfrc522.PCD_Init();

      return;
    }

    //PRINT DATA
    /*for (uint8_t i = 0; i < 16; i++)
    {
      if (buffer1[i] != 32)
      {
        Serial.print(buffer1[i],HEX);
      }
    }
    Serial.println("");*/

    //Reconstruct data

    //custom stuff
    

    int output = 0;

    for(int i = 0; i < 9; i++){
      output *= 10;
      output += (buffer1[i] - '0');
    }

    //Serial.println(F("Output "));
    //Serial.println(output);

    uint_fast32_t decrypted_output = encryption32(encryption_key).decrypt(output);

    //Serial.println(F("decrypted: "));
    //Serial.println(decrypted_output);

    RFIDControl.run(decrypted_output);
    




  

    //-------------------------------------------

    Serial.println(F("\n**End Reading**\n"));

    //delay(1000); //change value if you want to read cards faster

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  };
};

void setup() {
  //Set pinmode of the rgb pins
  pinMode(ledPinBlue, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinRed, OUTPUT);

  //buzzer pin out
  pinMode(buzzerPin, OUTPUT);
  
  //begin serial for debuging
  Serial.begin(9600);

  //RC522
  SPI.begin();
  mfrc522.PCD_Init();

  //SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  //RFID_Storage for person_data initialised by server
  RFID_Storage storage;

  //temperary server 
  Server_temp server(storage);

  //*todo : make html request on esp8266 for person_data in json form.

  //key used to decode the id on the nfc tag
  uint_fast32_t key = 0x14D2405C;

  //init nfc control
  RFID_Control rfidControl(server, storage, key);

  //init nfc reader
  nfc_reader nfcReader(mfrc522, rfidControl, key);

  //set the led to off for initialisation
  setRGB(0,0,0);

  //set buzzerPin to off for initialisation
  digitalWrite(buzzerPin, LOW);

  //draw idle screen on oled
  draw_idle_screen(barName, sesionName);

  //while true: read nfc
  do{
    nfcReader.read();
  }while(true);
}

void loop() {
  while(true){};
}
