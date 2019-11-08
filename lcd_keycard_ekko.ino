#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

#define RST_PIN 5
#define SS_PIN 53
const int buzzPin = 10;
const int redPin = 13;
const int greenPin = 12;
const int trigPin = 36;
const int echoPin = 37;
const int buttonPin = 2;

MFRC522::MIFARE_Key key;

byte readCard[4];
String tagIDD = "";
String MasterTag = "302108B";
String tagID = "";
String hiddenWord = "";
String passTry = "";

String card_id = "";
String card_name = "";
String card_pin = "";
int current_card = 0;
const int delMil = 100;
int prevMil = 0;
int buttonState = LOW;
volatile int pw_count = 0;

//implementer privat kode
int block = 2;
int pinBlock = 4;
String pinCode = "";




String cardID[10][3];


//This array is used for reading out a block.
byte readbackblock[18];


// Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup()
{
  // Initiating
  Serial.begin(9600);
  SPI.begin(); // SPI bus
  mfrc522.PCD_Init(); // MFRC522
  lcd.begin(16, 2); // LCD screen
  lcd.backlight();

  lcd.clear();
  lcd.print(" Access Control ");
  lcd.setCursor(0, 1);
  lcd.print("Scan Your Card>>");

  pinMode(buzzPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buttonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), pwb, FALLING);


  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  for (int i = 0; i < 217; i = i + 24) {  //Main loop writing to EEPROM
    if (EEPROM.read(i) != 0) {
      static int a = 0;
      int k = i;
      int l = k + 8;
      int t = l + 4;
      int s = t + 10;
      int j = 0;
      int m = 0;
      int n = 0;
      card_id = "";
      card_pin = "";
      card_name = "";

      for (int q = t; q < s; q++) {
        card_id.concat(char(EEPROM.read(q)));
      }
      for (int p = l; p < t; p++) {
        card_pin.concat(char(EEPROM.read(p)));
      }
      for (int i = k; i < l; i++) {
        //if (EEPROM.read(i) != 0) {
        card_name.concat(char(EEPROM.read(i)));
        //}
      }
      //i = 73;
      cardID[a][0] = card_name;
      cardID[a][1] = card_pin;
      cardID[a][2] = card_id;
      Serial.println("card_name: " + cardID[a][0]);
      Serial.println("card_pin: " + cardID[a][1]);
      Serial.println("card_id: " + cardID[a][2]);
      Serial.print("\n");
      a++;
    }
  }


}

void loop()
{
  //Wait until new tag is available
  pw_count = 0;
  while (getID(block, readbackblock))
  {
    lcd.clear();
    lcd.setCursor(0, 0);

    if (masterID() == true) {
      Serial.println();
      password();
      Serial.println(passTry);
      if (cardID[current_card][1] == passTry) {

        lcd.clear();
        lcd.print(" Gyldig!");
        tone(buzzPin, 2000);
        delay(100);
        noTone(buzzPin);
        digitalWrite(greenPin, HIGH);
        digitalWrite(redPin, LOW);
        lcd.setCursor(0, 1);
        lcd.print(" ID : ");
        lcd.print(cardID[current_card][2]);
        passTry = "";
      }
      else {
        lcd.clear();
        lcd.print(" Wrong password!");
        tone(buzzPin, 200);
        digitalWrite(redPin, LOW);
        delay(100);
        digitalWrite(redPin, HIGH);
        noTone(buzzPin);
        delay(50);
        tone(buzzPin, 200);
        digitalWrite(redPin, LOW);
        delay(100);
        noTone(buzzPin);
        digitalWrite(greenPin, LOW);
        digitalWrite(redPin, HIGH);
        delay(100);
        digitalWrite(redPin, LOW);
        passTry = "";
      }


    }
    else
    {
      lcd.print(" Access Denied!");
      tone(buzzPin, 200);
      digitalWrite(redPin, LOW);
      delay(100);
      digitalWrite(redPin, HIGH);
      noTone(buzzPin);
      delay(50);
      tone(buzzPin, 200);
      digitalWrite(redPin, LOW);
      delay(100);
      noTone(buzzPin);
      digitalWrite(greenPin, LOW);
      digitalWrite(redPin, HIGH);
    }



    delay(2000);
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, LOW);

    lcd.clear();
    lcd.print(" Access Control ");
    lcd.setCursor(0, 1);
    lcd.print("Scan Your Card>>");
  }

}

//Read new tag if available
int getID(int blockNumber, byte arrayAddress[])
{


  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return false;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
    return false;
  }


  int largestModulo4Number = blockNumber / 4 * 4;
  int trailerBlock = largestModulo4Number + 3; //determine trailer block for the sector

  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  byte buffersize = 18;
  status = mfrc522.MIFARE_Read(blockNumber, arrayAddress, &buffersize);//&buffersize is a pointer to the buffersize variable; MIFARE_Read requires a pointer instead of just a number


  tagID = "";
  hiddenWord = "";

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


  for (int i = 0; i < 16; i++) {
    hiddenWord.concat(String((char)arrayAddress[i]));

    delay(50);
  }
  delay(200);


  mfrc522.PCD_Init();
  return true;
}
String password() {
  unsigned long duration, cm, m, mm;
  int lyd = 0;
  int d = 50;
  passTry = "";

  bool pw_sound = true;
  while (pw_sound = true) {
    buttonState = digitalRead(buttonPin);
    // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
    // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);


    // Convert the time into a distance
    mm = ((duration / 2) * 334 ) * 0.001;
    cm = mm / 10;


    if (cm > 1 and cm <= 8) {
      lyd = 1;
    }
    else if (cm > 8 and cm <= 12) {
      lyd = 2;
    }
    else if (cm > 12 and cm <= 16) {
      lyd = 3;
    }
    else if (cm > 16 and cm <= 20) {
      lyd = 4;
    }
    else if (cm > 20 and cm <= 24) {
      lyd = 5;
    }
    else if (cm > 24 and cm <= 28) {
      lyd = 6;
    }
    else if (cm > 28 and cm <= 32) {
      lyd = 7;
    }
    else if (cm > 32 and cm <= 36) {
      lyd = 8;
    }
    else if (cm > 36 and cm <= 40) {
      lyd = 9;
    }

    else {
      lyd = 0;
    }
    switch (lyd) {
      case 0:
        noTone(buzzPin);
        lcd.setCursor(0, 0);
        lcd.print('0');
        break;
      case 1:
        tone(buzzPin, 327.0 );
        delay(d);
        lcd.setCursor(0, 0);
        lcd.print('1');
        break;
      case 2:
        tone(buzzPin, 367.1);
        delay(d);
        lcd.setCursor(0, 0);
        lcd.print('2');
        break;
      case 3:
        tone(buzzPin, 412.0);
        delay(d);
        lcd.setCursor(0, 0);
        lcd.print('3');
        break;
      case 4:
        tone(buzzPin, 436.5);
        delay(d);
        lcd.setCursor(0, 0);
        lcd.print('4');
        break;
      case 5:
        tone(buzzPin, 490.0);
        delay(d);
        lcd.setCursor(0, 0);
        lcd.print('5');
        break;
      case 6:
        tone(buzzPin, 550.0);
        delay(d);
        lcd.setCursor(0, 0);
        lcd.print('6');
        break;
      case 7:
        tone(buzzPin, 617.4);
        delay(d);
        lcd.setCursor(0, 0);
        lcd.print('7');
        break;
      case 8:
        tone(buzzPin, 654.1);
        delay(d);
        lcd.setCursor(0, 0);
        lcd.print('8');
        break;
      case 9:
        tone(buzzPin, 734.2);
        delay(d);
        lcd.setCursor(0, 0);
        lcd.print('9');
        break;
      default:
        noTone(buzzPin);
        delay(10);
        break;

    }


    if (pw_count == 1 && passTry.length() < 1) {
      passTry.concat(String(lyd));
      lcd.setCursor(0, 1);
      lcd.print(passTry);
      Serial.println("pw input: " + passTry);
    }
    if (pw_count == 2 && passTry.length() < 2) {
      passTry.concat(String(lyd));
      lcd.setCursor(0, 1);
      lcd.print(passTry);
      Serial.println("pw input: " + passTry);
    }
    if (pw_count == 3 && passTry.length() < 3) {
      passTry.concat(String(lyd));
      lcd.setCursor(0, 1);
      lcd.print(passTry);
      Serial.println("pw input: " + passTry);
    }
    if (pw_count == 4 && passTry.length() < 4) {
      passTry.concat(String(lyd));
      lcd.setCursor(0, 1);
      lcd.print(passTry);
      Serial.println("pw input: " + passTry);
    }
    if (pw_count >= 5) {
      //pw_sound = false;
      return passTry;
    }
  }
}

bool masterID() {
  Serial.println("dette er tagID: " + tagID);
  for (int i = 0; i < 10; i++) {

    if (tagID == cardID[i][0]) {

      current_card = i;
      return true;
    }
  }
  return false;
}

void pwb() {

  if (millis() > prevMil + delMil) {
    buttonState = !buttonState;
    Serial.println("BUTTONSTATE CHANGE");
    prevMil = millis();
    pw_count++;

  }
}
