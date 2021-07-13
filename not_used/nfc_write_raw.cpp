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

void setup() {
  Serial.begin(9600);        // Initialize serial communications with the PC
  SPI.begin();               // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card
  Serial.println(F("Write personal data on a MIFARE PICC "));
}

void loop() {

  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print(F("Card UID:"));    //Dump UID
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.print(F(" PICC type: "));   // Dump PICC type
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  byte buffer[34];
  byte block;
  MFRC522::StatusCode status;
  byte len;

  /*Serial.setTimeout(20000L) ;     // wait until 20 seconds for input from serial
  // Ask personal data: Family name
  Serial.println(F("Type Family name, ending with #"));
  len = Serial.readBytesUntil('#', (char *) buffer, 30) ; // read family name from serial*/



  /*String dataToWrite = "PPPPPPPPAAAAAAAA";
  
  byte buffer2[dataToWrite.length()];
  
  for(int i = 0; i < dataToWrite.length(); i++){
    buffer2[i] = dataToWrite[i];
  }

  Serial.println(F("buffer2:"));
  for(byte b : buffer2){
      Serial.print(b);
      Serial.print(F(" "));
  }
  Serial.println(F(""));*/

  /*Serial.println(F("buffer1:"));
  for(int i = 0; i < 34; i++){
      Serial.print(buffer[i]);
      Serial.print(F(" "));
  }
  Serial.println(F(""));
  
  for (byte i = len; i < 30; i++) buffer[i] = ' ';     // pad with spaces
  for (byte i = len; i < dataToWrite.length(); i++) buffer2[i] = ' ';     // pad with spaces*/




  uint32_t encryption_key = 0xec265e81;

  uint32_t dataToWriteInt = 3;
  byte bufferInt[16];

  Serial.println(F("dataToWriteInt: "));
  Serial.println(dataToWriteInt, HEX);

  dataToWriteInt = encryption32(encryption_key).encrypt(dataToWriteInt);

  
  //using masks and bitshifts to spit a deciamal int into 4 bytes
  bufferInt[3] = dataToWriteInt & 0x000000FF;
  bufferInt[2] = (dataToWriteInt & 0x0000FF00) >> 8;
  bufferInt[1] = (dataToWriteInt & 0x00FF0000) >> 16;
  bufferInt[0] = (dataToWriteInt & 0xFF000000) >> 24;
  
  
  
  
  //clean byte 4 - 15
  for(int i = 4; i < 16; i++){
    bufferInt[i] = 0;
  }

  /*Serial.println(F("bufferInt:"));
  for(byte b : bufferInt){
      Serial.print(b);
      Serial.print(F(" "));
  }
  Serial.println(F(""));*/

  Serial.println(F("encrypted: "));
  Serial.println(dataToWriteInt, HEX);
  
  block = 4;
  //Serial.println(F("Authenticating using key A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Write block
  status = mfrc522.MIFARE_Write(block, bufferInt, 16 );
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));

  
  
  /*block = 1;
  //Serial.println(F("Authenticating using key A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("PCD_Authenticate() success: "));

  // Write block
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));

  block = 2;
  //Serial.println(F("Authenticating using key A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Write block
  status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));*/


  Serial.println(" ");
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD

}