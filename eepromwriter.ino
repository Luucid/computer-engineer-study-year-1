#include <Wire.h>
#include <SPI.h>      //include the SPI bus library
#include <MFRC522.h>  //include the RFID reader library
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
//mosi = 51; miso = 50; sck = 52; sda = 53;

#define RST_PIN 5  //reset pin
#define SS_PIN 53  //slave select pin

MFRC522::MIFARE_Key key;          //create a MIFARE_Key struct named 'key', which will hold the card information
MFRC522 mfrc522(SS_PIN, RST_PIN);  // instatiate a MFRC522 reader object.


//this is the block number we will write into and then read.
int block = 2;
int* blockPtr = &block;


//This array is used for reading out a block.
byte readbackblock[18];


String cardID[10][3];
String tagID = "";
String val = "";



void setup()
{
  Serial.begin(9600);        // Initialize serial communications with the PC
  SPI.begin();               // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card (in case you wonder what PCD means: proximity coupling device)
  lcd.begin(16, 2); // LCD screen
  lcd.backlight();

  lcd.clear();
  lcd.print("scan your card");
  int k = 8;
  int l = 0;
  delay(10);
  // Prepare the security key for the read and write functions.
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;  //keyByte is defined in the "MIFARE_Key" 'struct' definition in the .h file of the library
  }

  for (int i = 0; i < 10; i++) {
    for (int i = l; i < k; i++) {
      val.concat(char(EEPROM.read(i)));
    }
    cardID[i][0] = val;
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

  Serial.print("\n");

  delay(3000);
  lcd.clear();
  lcd.print("Scan your card");
  mfrc522.PCD_Init();
}





//Read specific block
int readBlock(int blockNumber, byte arrayAddress[])
{
  Serial.print("\n");
  lcd.clear();
  val = "";
  tagID = "";
  int k = 0;
  int l = 0;


  int largestModulo4Number = blockNumber / 4 * 4;
  int trailerBlock = largestModulo4Number + 3; //determine trailer block for the sector

  //authentication of the desired block for access
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));

 /* if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed (read): ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 3; //return the value 3 as error message.
  }*/

  //reading a block
  byte buffersize = 18;
  status = mfrc522.MIFARE_Read(blockNumber, arrayAddress, &buffersize);
  
  /*if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE_read() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 4;//return "4" as error message
  }*/


  for ( uint8_t i = 0; i < 4; i++) { // The MIFARE PICCs that we use have 4 byte UID

    tagID.concat(String(arrayAddress[i], HEX)); // Adds the 4 bytes in a single String variable

  }
  if (tagID.length() < 8) {
    int y = tagID.length() + 1;
    for (int i = y; i <= 8; i++) {
      tagID.concat('A');
    }
  }
  tagID.toUpperCase();



  for (int i = 0; i < 10; i++) { //checks for duplicates the array containing card register.
    if (tagID == cardID[i][0]) {
      Serial.println("duplicate found");
      lcd.print("duplicate found");
      return 0;
    }
  }

  Serial.println("please enter your name, maximum length: 10");
  while (!Serial.available());
  String cardName = Serial.readString();
  if (cardName.length() > 10) {
    Serial.println("Your name is too long, scan your card to try again");
    return 0;
  }
  if (cardName.length() < 10) {
    int x = cardName.length() + 1;
    for (int i = x; i <= 10; i++) {
      cardName.concat(' ');

    }
  }
  Serial.println(cardName);
  Serial.println(cardName.length());



  Serial.println("please enter your new pincode, 4 digits");
  while (!Serial.available());
  String pinCode = Serial.readString();
  if (pinCode.length() != 4) {
    Serial.println("wrong number of digits, scan your card to try again");
    return 0;
  }


  Serial.print("\n");
  for (int i = 0; i < 10; i++) {  //adds new cards to array
    if (cardID[i][0] == 0) {
      Serial.println("New card added!");
      lcd.print("New card added!");
      cardID[i][2] = cardName;
      cardID[i][1] = pinCode;
      cardID[i][0] = tagID;
      Serial.println("Card owner: " + cardID[i][2]);
      Serial.println("Pin code: " + cardID[i][1]);
      Serial.println("Card ID: " + cardID[i][0]);
      break;
    }
  }

  Serial.print("\n");


  for (int i = 0; i < 73; i = i + 8) {  //Main loop writing to EEPROM
    if (EEPROM.read(i) == 0) {
      int k = i;
      int l = k + 8;
      int t = l + 4;
      int s = t + 10;
      int j = 0;
      int m = 0;
      int n = 0;

      Serial.print("\t");
      Serial.println("EEPROM INDEX FOR CARD_NAME");
      for (int q = t; q < s; q++) {
        Serial.print(q);
        Serial.print("\t");
        EEPROM.put(q, cardName[n]);
        n++;

      }

      Serial.print("\n\n");
      Serial.print("\t");
      Serial.println("EEPROM INDEX FOR PIN");
      for (int p = l; p < t; p++) {           //writing card data to EEPROM
        Serial.print(p);
        Serial.print("\t");
        EEPROM.put(p, pinCode[m]);
        m++;
      }
      Serial.print("\n\n");
      Serial.print("\t");


      Serial.println("EEPROM INDEX FOR CARD_DATA");
      for (int i = k; i < l; i++) {             //writing pin to EEPROM
        EEPROM.put(i, tagID[j]);
        delay(1);
        Serial.print(i);
        Serial.print("\t");
        j++;
      }
      i = 73;
    }
  }
}

void duplicate_check() {    //init at startup, if duplicates somehow exists in EEPROM, set them to 0.
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      if (i == j) continue;
      if (cardID[i][0] != 0 && cardID[i][0] == cardID[j][0]) {
        for (int k = 0; k < 8; k++) {
          int e_index = i * 8 + k;
          Serial.println("duplikat funnet og slettet ved start-indeks [" + (String)e_index + ']');

        }
      }
    }
  }
}

//lag funksjon så bruker kan skrive inn egen pin og ID ved registrering
