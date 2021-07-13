#include "barTapNfc.h"

barTap::nfc::nfc(){
  SPI.begin();
  mfrc522.PCD_Init();
  delay(4);
}

void barTap::nfc::getInfo(){
  mfrc522.PCD_DumpVersionToSerial();
}

int barTap::nfc::read(){
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return -1;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return -2;
  }

  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  MFRC522::StatusCode status;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, 4, &key, &(mfrc522.uid)); //line 834
  
  if(status != MFRC522::STATUS_OK){
    Serial.println("NOT OK");
    Serial.println(mfrc522.GetStatusCodeName(status));
  }

  byte buffer0[18];

  byte block = 4;
  byte len = 18;

  status = mfrc522.MIFARE_Read(block, buffer0, &len);
  if(status != MFRC522::STATUS_OK){
    Serial.println("NOT OK");
    Serial.println(mfrc522.GetStatusCodeName(status));
  }

  //place to read block 4:
  byte buffer1[18];

  block = 5;
  len = 18;

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if(status != MFRC522::STATUS_OK){
    Serial.println("NOT OK");
    Serial.println(mfrc522.GetStatusCodeName(status));
  }

  String s;

  for (uint8_t i = 9; i < 16; i++){
    s += (buffer0[i] - '0');
  }
  for (uint8_t i = 0; i < 2; i++){
    s += (buffer1[i] - '0');
  }

  
  Serial.println();
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  
  return s.toInt(); 
}