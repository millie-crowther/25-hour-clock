int datapin  = A0;
int clockpin = A2;
int latchpin = A1;

//data for pins
int pins[14] = {
  -1, -1, -1, -1, A5, -1, 4,
  13, 6, A4, 9, -1, -1, 10
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

  time_of_last_update = millis();
}

int level(boolean v){
  if (v){
    return LOW;
  } else {
    return HIGH;
  }
}

boolean get_value(int number, int segment){
  if (segment == 0){
    return val != 1 && val != 4;

  } else if (segment == 1){
    return val != 5 && val != 6;

  } else if (segment == 2){
    return val != 2;

  } else if (segment == 3){
    return val != 1 && val != 4 && val != 7;

  } else if (segment == 4){
    return val == 0 || val == 2 || val == 6 || val == 8;

  } else if (segment == 5){
    return val != 1 && val != 2 && val != 3 && val != 7;

  } else {
    return val != 0 && val != 1 && val != 7;
  }
}

void output_pins_digit(int digit, int val){
  if (digit == 4 || digit == 5){
    int offset = (digit - 4) * 7;
    for (int segment = 0; segment < 7; segment++){
      int p = pins[segment + offset];
      if (p != -1){
        digitalWrite(p, level(get_value(val, segment)));
      }
    }
  }

  if (digit == 3){
    digitalWrite(A3, segments[1]);
    digitalWrite(7, segments[2]);
  }
}

void output_shift_digit(int digit, int val){
  for (int segment = 0; segment < 7; segment++){
    if (get_value(val, segment)){
      int bit = bits[digit * 7 + segment];
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
  shiftOut(datapin, clockpin, MSBFIRST, ~data[2]);
  shiftOut(datapin, clockpin, MSBFIRST, ~data[1]);
  shiftOut(datapin, clockpin, MSBFIRST, ~data[0]);
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

