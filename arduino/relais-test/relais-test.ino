
void setup() {
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
}

int count = 0;

void loop() {
  int mode = 1;

  for (int i = 5; i < 13; i++) {
    for (int j = 5; j < 13; j++) {
      switch (mode) {
      case 1:
            digitalWrite(j, i != j);
            break;
      case 2:
            digitalWrite(j, random() & 1);
            break;
      }
    }

    digitalWrite(13, HIGH);
    delay(50);
    digitalWrite(13, LOW);
    delay(200);  
  }
}
