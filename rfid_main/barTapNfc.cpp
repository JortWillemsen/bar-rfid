#include "barTapNfc.h"

barTap::nfc::nfc(){
  SPI.begin();
  mfrc522.PCD_Init();
  delay(4);
}

void barTap::nfc::getInfo(){
  mfrc522.PCD_DumpVersionToSerial();
}

int barTap::nfc::readClassic(){
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
    //Serial.println("NOT OK authenticate with key B");
    //Serial.println(mfrc522.GetStatusCodeName(status));

    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834

    if(status != MFRC522::STATUS_OK){
      //Serial.println("NOT OK authenticate with key A");
      //Serial.println(mfrc522.GetStatusCodeName(status));
      return -1;
    }
  }

  byte buffer0[18];

  byte block = 4;
  byte len = 18;

  status = mfrc522.MIFARE_Read(block, buffer0, &len);
  if(status != MFRC522::STATUS_OK){
    //Serial.println("NOT OK read");
    //Serial.println(mfrc522.GetStatusCodeName(status));
    return -1;
  }

  //place to read block 4:
  byte buffer1[18];

  block = 5;
  len = 18;

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if(status != MFRC522::STATUS_OK){
    //Serial.println("NOT OK");
    //Serial.println(mfrc522.GetStatusCodeName(status));
    return -1;
  }

  String s;

  for (uint8_t i = 9; i < 16; i++){
    s += (buffer0[i] - '0');
  }
  for (uint8_t i = 0; i < 2; i++){
    s += (buffer1[i] - '0');
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  
  return s.toInt(); 
}

int barTap::nfc::readUL(){
    if ( ! mfrc522.PICC_IsNewCardPresent()){
        return -1;
    }

    MFRC522::StatusCode status;

    byte buffer0[18];  //data transfer buffer (16+2 bytes data+CRC)
    byte size = sizeof(buffer0);

    //output string
    String outputString = "";

    delay(100);

    //data in 4 block is readed at once.
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(0x06, buffer0, &size);
    if (status != MFRC522::STATUS_OK) {
        //Serial.print(F("MIFARE_Read() failed: "));
        //Serial.println(mfrc522.GetStatusCodeName(status));
        return -1;
    }

    for(int i = 1; i < 10; i++){
        outputString += (buffer0[i] - '0');
    }


    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return outputString.toInt();
}