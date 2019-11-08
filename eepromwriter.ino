#include <Wire.h>
#include <SPI.h>      //include the SPI bus library
#include <MFRC522.h>  //include the RFID reader library
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

#define RST_PIN 5  //reset pin
#define SS_PIN 53  //slave select pin

MFRC522::MIFARE_Key key;          //create a MIFARE_Key struct named 'key', which will hold the card information
MFRC522 mfrc522(SS_PIN, RST_PIN);  // instatiate a MFRC522 reader object.


//this is the block number we will write into and then read.
int block = 2;
int* blockPtr = &block;


//This array is used for reading out a block.
byte readbackblock[18];

String cardID[10];
String tagID = "";
String val = "";
int k = 8;
int l = 0;
bool startup = false;
void setup()
{
  Serial.begin(9600);        // Initialize serial communications with the PC
  SPI.begin();               // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card (in case you wonder what PCD means: proximity coupling device)
  lcd.begin(16, 2); // LCD screen
  lcd.backlight();

  lcd.clear();
  lcd.print("scan your card");
  delay(10);
  // Prepare the security key for the read and write functions.
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;  //keyByte is defined in the "MIFARE_Key" 'struct' definition in the .h file of the library
  }

  for (int i = 0; i < 10; i++) {
    for (int i = l; i < k; i++) {
      val.concat(char(EEPROM.read(i)));
    }
    cardID[i] = val;
    val = "";
    l = l + 8;
    k = k + 8;

  }
  l = 0;
  k = 8;

  duplicate_check();
}

void loop()
{
  
  if (Serial.available()) {
    char nextPrev = Serial.read();
    if (nextPrev == 'N' or nextPrev == 'n' && block < 60) {
      block += 2;
      Serial.println(block);
    }
    else if (nextPrev == 'P' or nextPrev == 'p' && block > 0) {
      block -= 2;
      Serial.println(block);
    }


  }
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  readBlock(block, readbackblock);


  //print the block contents
  Serial.print("card ID: ");
  for (int j = 0 ; j < 16 ; j++)
  {
    Serial.write (readbackblock[j]);
  }
  Serial.println("");
  Serial.print("\n");
 
  delay(3000);
  lcd.clear();
  lcd.print("Scan your card");
  mfrc522.PCD_Init();
}





//Read specific block
int readBlock(int blockNumber, byte arrayAddress[])
{
  lcd.clear();
  val = "";
  tagID = "";
  k = 0;
  l = 0;


  int largestModulo4Number = blockNumber / 4 * 4;
  int trailerBlock = largestModulo4Number + 3; //determine trailer block for the sector

  //authentication of the desired block for access
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed (read): ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 3; //return the value 3 as error message.
  }

  //reading a block
  byte buffersize = 18;
  status = mfrc522.MIFARE_Read(blockNumber, arrayAddress, &buffersize);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE_read() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 4;//return "4" as error message
  }
  

  for ( uint8_t i = 0; i < 4; i++) { // The MIFARE PICCs that we use have 4 byte UID

    tagID.concat(String(arrayAddress[i], HEX)); // Adds the 4 bytes in a single String variable

  }
  tagID.toUpperCase();

  for (int i = 0; i < 10; i++) {
    if (tagID == cardID[i]) {
      Serial.println("duplicate found");
      lcd.print("duplicate found");
      return 0;
    }
  }
  for (int i = 0; i < 10; i++) {
    if (cardID[i] == 0) {
      Serial.print("New card added! ID: ");
      Serial.println(cardID[i]);
      lcd.print("New card added!");
      cardID[i] = tagID;
      break;
    }
  }

  for (int i = 0; i < 73; i = i + 8) {
    if (EEPROM.read(i) == 0) {
      int k = i;
      int l = k + 8;
      int j = 0;
      Serial.print("writing to EEPROM[");
      Serial.print(i);
      Serial.println(']');
      for (int i = k; i < l; i++) {
        EEPROM.put(i, tagID[j]);
        delay(1);
       // val.concat(EEPROM.read(i));
        j++;
      }
      i = 73;
    }
  }
}

void duplicate_check() {
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      if (i == j) continue;
      if (cardID[i] != 0 && cardID[i] == cardID[j]) {
        for (int k = 0; k < 8; k++) {
          int e_index = i * 8 + k;
          Serial.println("duplikat funnet og slettet ved start-indeks [" + (String)e_index + ']');

        }
      }
    }
  }
}

//lag funksjon så bruker kan skrive inn egen pin og ID ved registrering
