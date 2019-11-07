//Jorgen W

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

const int clk = 10;
const int A = 22;
const int B = 37;
const int C = 41;
const int p_a = 49;
const int p_b = 53;
const int p_c = 52;
const int res = 48;

void setup() {

  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.backlight();
  pinMode(clk, OUTPUT);
  pinMode(A, INPUT);
  pinMode(B, INPUT);
  pinMode(C, INPUT);
  pinMode(p_a, OUTPUT);
  pinMode(p_b, OUTPUT);
  pinMode(p_c, OUTPUT);
  pinMode(res, OUTPUT);
  digitalWrite(clk, LOW);
  Serial.println("ready");
  Serial.println(' ');

  digitalWrite(p_a, HIGH);
  digitalWrite(p_b, HIGH);
  digitalWrite(p_c, HIGH);
  digitalWrite(res, HIGH);

  byte a = digitalRead(A);
  byte b = digitalRead(B);
  byte c = digitalRead(C);
  byte csum = c << 2;
  byte bsum = b << 1;
  byte sum = csum + bsum + a;

  lcd.clear();
  lcd.print(c);
  lcd.print(b);
  lcd.print(a);
  lcd.print(" = ");
  lcd.print(sum);
  Serial.print(c);
  Serial.print(b);
  Serial.print(a);
  Serial.print(" = ");
  Serial.println(sum);
}


void loop() {



  while (!Serial.available());
  char funk = Serial.read();
  if (funk == 'p') {
    Serial.println("choose preset bit: a, b or c");
    while (!Serial.available());
    char valg = Serial.read();
    if (valg == 'a') {
      digitalWrite(p_a, LOW);
      digitalWrite(p_a, HIGH);
      byte a = digitalRead(A);
      byte b = digitalRead(B);
      byte c = digitalRead(C);
      byte csum = c << 2;
      byte bsum = b << 1;
      byte sum = csum + bsum + a;

      lcd.clear();
      lcd.print(c);
      lcd.print(b);
      lcd.print(a);
      lcd.print(" = ");
      lcd.print(sum);
      Serial.print(c);
      Serial.print(b);
      Serial.print(a);
      Serial.print(" = ");
      Serial.println(sum);
    }
    if (valg == 'b') {
      digitalWrite(p_b, LOW);
      digitalWrite(p_b, HIGH);
      byte a = digitalRead(A);
      byte b = digitalRead(B);
      byte c = digitalRead(C);
      byte csum = c << 2;
      byte bsum = b << 1;
      byte sum = csum + bsum + a;

      lcd.clear();
      lcd.print(c);
      lcd.print(b);
      lcd.print(a);
      lcd.print(" = ");
      lcd.print(sum);
      Serial.print(c);
      Serial.print(b);
      Serial.print(a);
      Serial.print(" = ");
      Serial.println(sum);
    }
    if (valg == 'c') {
      digitalWrite(p_c, LOW);
      digitalWrite(p_c, HIGH);

      byte a = digitalRead(A);
      byte b = digitalRead(B);
      byte c = digitalRead(C);
      byte csum = c << 2;
      byte bsum = b << 1;
      byte sum = csum + bsum + a;

      lcd.clear();
      lcd.print(c);
      lcd.print(b);
      lcd.print(a);
      lcd.print(" = ");
      lcd.print(sum);
      Serial.print(c);
      Serial.print(b);
      Serial.print(a);
      Serial.print(" = ");
      Serial.println(sum);
    }
  }
  else if (funk == 'r') {
    digitalWrite(res, LOW);
    digitalWrite(res, HIGH);
    byte a = digitalRead(A);
    byte b = digitalRead(B);
    byte c = digitalRead(C);
    byte csum = c << 2;
    byte bsum = b << 1;
    byte sum = csum + bsum + a;

    lcd.clear();
    lcd.print(c);
    lcd.print(b);
    lcd.print(a);
    lcd.print(" = ");
    lcd.print(sum);
    Serial.print(c);
    Serial.print(b);
    Serial.print(a);
    Serial.print(" = ");
    Serial.println(sum);
  }
  else {
    klokke();
  }

}

void klokke() {
  digitalWrite(clk, HIGH);
  digitalWrite(clk, LOW);

  byte a = digitalRead(A);
  byte b = digitalRead(B);
  byte c = digitalRead(C);
  byte csum = c << 2;
  byte bsum = b << 1;
  byte sum = csum + bsum + a;

  lcd.clear();
  lcd.print(c);
  lcd.print(b);
  lcd.print(a);
  lcd.print(" = ");
  lcd.print(sum);
  Serial.print(c);
  Serial.print(b);
  Serial.print(a);
  Serial.print(" = ");
  Serial.println(sum);
}
