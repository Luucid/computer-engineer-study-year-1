///////////NUMBERS ZERO to NINE/////////////////
int numbers [10][9] = {
  {0, 1, 2, 3, 4, 5, 6, 7},
  {1, 3, 6, 7},
  {2, 2, 3, 0, 4, 5, 7},
  {3, 2, 3, 0, 6, 5, 7},
  {4, 1, 0, 3, 6, 7},
  {5, 2, 1, 0, 6, 5, 7},
  {6, 1, 2, 0, 4, 5, 6, 7},
  {7, 2, 3, 6, 7},
  {8, 0, 1, 2, 3, 4, 5, 6, 7},
  {9, 1, 2, 3, 0, 6, 7}
};
/////////////////////////////////////////////////

const int antPins = 8;
int led_counter[antPins];
int i = 0;

void setup()
{
  Serial.begin(9600);
  led_setup();
  for (i = 0; i < 8; i++) {
    pinMode(led_counter[i], OUTPUT);
  }
}

void loop()
{
  while (!Serial.available());
  int verdi = Serial.parseInt();
  if (verdi >= 0 && verdi <= 9) {
    while_tall(verdi);
    delay(500);
    for (int i = 0; i < 8; i++) {
      digitalWrite(led_counter[i], LOW);

    }
  }
}

void while_tall(int verdi) {
  for (int i = 0; i < 10; i++) {
    if (verdi == numbers[i][0]) {
      for (int j = 1; j < 9; j++) {        
        digitalWrite(led_counter[numbers[i][j]], HIGH);
        if (j == 8) {
          i = 10;
        }
        if (verdi == 0 || verdi == 1 || verdi == 7) {
          digitalWrite(led_counter[0], LOW);
        }
      }
    }
  }
}

void led_setup() {
  for (int i = 0; i < 8; i++) {
    static int pin = 13;
    led_counter[i] = pin;
    if (pin > 5) {
      pin--;
    }
  }
}
