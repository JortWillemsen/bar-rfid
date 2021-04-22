#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance


class encryption32{
  private:
    uint32_t key;

  public:
    encryption32(uint32_t key) : key(key) {}

    uint32_t encrypt(const uint32_t& id){
        return id^key;
    }

    uint32_t decrypt(const uint32_t& id){
        return key^id;
    }
};

class RFID_Control{
  public:
  void run(uint_fast32_t input){
    Serial.println("RFID_Control");
    Serial.println(input);
    Serial.println("");
  }
};

class nfc_reader{
  private:
  MFRC522& mfrc522;
  RFID_Control& RFIDControl;

  uint32_t encryption_key = 0x14D2405C;

  public:
  nfc_reader(MFRC522& mfrc522, RFID_Control& RFIDControl): 
    mfrc522(mfrc522),
    RFIDControl(RFIDControl)
  {}

  

  void read(){
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
      return;
    }

    Serial.println(F("**Card Detected:**"));

    //-------------------------------------------

    mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

    //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex

    //-------------------------------------------

    //Serial.print(F("Name: "));

    byte buffer1[18];

    block = 4;
    len = 18;

    //------------------------------------------- READ encryted id
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Authentication failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    status = mfrc522.MIFARE_Read(block, buffer1, &len);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Reading failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    //PRINT DATA
    for (uint8_t i = 0; i < 16; i++)
    {
      if (buffer1[i] != 32)
      {
        Serial.print(buffer1[i],HEX);
      }
    }
    Serial.println("");

    //Reconstruct data

    //custom stuff
    

    int output = 0;

    for(int i = 0; i < 9; i++){
      output *= 10;
      output += (buffer1[i] - '0');
    }

    Serial.println(F("Output "));
    Serial.println(output);

    uint_fast32_t decrypted_output = encryption32(encryption_key).decrypt(output);

    Serial.println(F("decrypted: "));
    Serial.println(decrypted_output);

    RFIDControl.run(decrypted_output);
    




  

    //-------------------------------------------

    Serial.println(F("\n**End Reading**\n"));

    delay(1000); //change value if you want to read cards faster

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  };
};


void setup() {
  Serial.begin(9600);                                           // Initialize serial communications with the PC
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card

  RFID_Control RFIDControl;
  nfc_reader nfcReader(mfrc522, RFIDControl);

  Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read

  do{
    nfcReader.read();
  }while(true);
}


void loop() {
  //
}

