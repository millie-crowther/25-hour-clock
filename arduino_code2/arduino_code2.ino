int datapin = 11;
int clockpin = 12;
int latchpin = 8;

//data for pins
int pins[14] = {
  5, 2, 3, -1, A5, A2, 4,
  13, 6, A4, 9, A0, A1, 10
};

// data for shift registers
byte data[3];
int bits[28] = {
  4, -1, 5, -1, 12, 13, 6,
  15, 11, 2, 8, 0, 7, 14,
  3, 19, 17, -1, 9, 10, 1,
  18, 16, -1, 23, 21, 20, 22
};

int time_at_last_update;
unsigned long time_of_last_update;

void setup(){
  Serial.begin(9600);
  for (int i = 2; i < 14; i++){
    pinMode(i, OUTPUT);
  }
  
  for (int i = A0; i <= A5; i++){
    pinMode(i, OUTPUT);
  }

  for (int i = 0; i < 3; i++){
    data[i] = 0;
  }

  time_of_last_update = millis();
}

int level(boolean v){
  if (v){
    return LOW;
  } else {
    return HIGH;
  }
}

void output_pins_digit(int digit, int val){
  boolean segments[6];
  segments[0] = val != 1 && val != 4;
  segments[1] = val != 5 && val != 6;
  segments[2] = val != 2;
  segments[3] = val != 1 && val != 4 && val != 7;
  segments[4] = val == 0 || val == 2 || val == 6 || val == 8;
  segments[5] = val != 1 && val != 2 && val != 3 && val != 7;
  segments[6] = val != 0 && val != 1 && val != 7;

  if (digit == 4 || digit == 5){
    int offset = (digit - 4) * 7;
    for (int i = 0; i < 7; i++){
      if (pins[i + offset] != -1){
        digitalWrite(pins[i + offset], level(segments[i]));
      }
      if (digit == 5 && i == 6){
        Serial.print("j=");
        Serial.print(pins[i+offset]);
        Serial.print("s=");
        Serial.println(segments[6]);
      }
    }
  }

  if (digit == 3){
    digitalWrite(A3, segments[1]);
    digitalWrite(7, segments[2]);
  }
}

void output_shift_digit(int digit, boolean val){
  boolean segments[7];
  segments[0] = val != 1 && val != 4;
  segments[1] = val != 5 && val != 6;
  segments[2] = val != 2;
  segments[3] = val != 1 && val != 4 && val != 7;
  segments[4] = val == 0 || val == 2 || val == 6 || val == 8;
  segments[5] = val != 1 && val != 2 && val != 3 && val != 7;
  segments[6] = val != 0 && val != 1 && val != 7;

  for (int i = 0; i < 7; i++){
    if (segments[i]){
      int bit = bits[digit * 7 + i];
      if (bit != -1){
        data[bit / 8] |= 1 << (bit % 8);
      }
    }
  }
}

void output(){
  int t = current_time();
  int h = (t / 3600) % 25;
  int m = (t / 60) % 60;
  int s = t % 60;

  int digits[6];
  digits[0] = h / 10;
  digits[1] = h % 10;
  digits[2] = m / 10;
  digits[3] = m % 10;
  digits[4] = s / 10;
  digits[5] = s % 10;

  for (int i = 3; i < 6; i++){
    output_pins_digit(i, digits[i]);
  }

  // do shift register output
  for (int i = 0; i < 3; i++){
    data[i] = 0;
  }

  for (int digit = 0; digit < 4; digit++){
    output_shift_digit(digit, digits[digit]);
  }

  digitalWrite(latchpin, LOW);
  for (int i = 2; i >= 0; i--){
    shiftOut(datapin, clockpin, MSBFIRST, ~data[i]);
  }
  digitalWrite(latchpin, HIGH);
}

int current_time(){
  return time_at_last_update + (millis() - time_of_last_update) / 1000;
}

void loop(){  
  String t = "";
  while (Serial.available()){
    char c = Serial.read();
    if (c == '.'){
      time_at_last_update = t.toInt();
      time_of_last_update = millis();
    } else {
      t.concat(c);
    }
    delay(10);
  }

  delay(200);

  output();
}

