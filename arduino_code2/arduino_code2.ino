int datapin1  = A0;
int clockpin1 = A2;
int latchpin1 = A1;

int datapin2  = A3;
int clockpin2 = A4;
int latchpin2 = A5;

//data for pins
int pins[14] = {
  -1, -1, -1, -1, A5, A2, 4,
  13, 6, A4, 9, A0, A1, 10
};

// data for shift registers
byte data[4];
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

  for (int i = 2; i < 4; i++){
    digitalWrite(i, LOW);
  }

  for (int i = 4; i < 7; i++){ 
    digitalWrite(i, HIGH);
  }

  
  for (int i = A0; i <= A5; i++){
    pinMode(i, OUTPUT);
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
  boolean segments[7];
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
      int p = pins[i + offset];
      if (p != -1){
        //digitalWrite(p, level(segments[i]));
      }
    }
  }

  if (digit == 3){
    //digitalWrite(A3, segments[1]);
    //digitalWrite(7, segments[2]);
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
        //data[bit / 8] |= 1 << (bit % 8);
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

  digitalWrite(latchpin1, LOW);
  shiftOut(datapin1, clockpin1, MSBFIRST, ~data[3]);
  shiftOut(datapin1, clockpin1, MSBFIRST, ~data[2]);
  digitalWrite(latchpin1, HIGH);
  
  digitalWrite(latchpin2, LOW);
  shiftOut(datapin2, clockpin2, MSBFIRST, ~data[1]);
  shiftOut(datapin2, clockpin2, MSBFIRST, ~data[0]);
  digitalWrite(latchpin2, HIGH);
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

