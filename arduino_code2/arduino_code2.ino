int datapin  = 5;
int clockpin = 2;
int latchpin = 3;

//data for pins
int pins[42] = {
  -1, -1, -1, -1, -1, -1, -1, // 0
  -1, -1, -1, -1, -1, -1, -1, // 1
  -1, A5,  4, -1, -1, -1, -1, // 2
  A2, 11, -1, A0, A1, A3, A4, // 3
  -1, 10, -1, -1, 13, 12, -1, // 4
  -1, -1,  9, -1,  8,  6, -1, // 5
};

// data for shift registers
int bits[42] = {
  12, -1, 14, -1,  4,  3, 13, // 0
   6,  2, 10,  0,  8, 15,  5, // 1
   9, -1, -1, -1,  7,  1, 11, // 2
  -1, -1, 19, -1, -1, -1, -1, // 3
  18, -1, 16, -1, -1, -1, 17, // 4
  22, 20, -1, 21, -1, -1, 23, // 5
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

boolean get_value(int val, int segment){
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

void output(){
  int t = time_at_last_update + ((int) ((millis() - time_of_last_update) / 1000));
  int h = (t / 3600) % 25;
  int m = (t / 60) % 60;
  int s = t % 60;

  int digits[6] = {
    h / 10, h % 10,
    m / 10, m % 10,
    s / 10, s % 10
  };

  // do shift register output
  byte data[3] = { 0, 0, 0 };

  for (int digit = 0; digit < 6; digit++){
    for (int segment = 0; segment < 7; segment++){
      int i = digit * 7 + segment;
    
      boolean isOn = get_value(digits[digit], segment);
    
      if (pins[i] != -1){
        digitalWrite(pins[i], isOn ? LOW : HIGH);
      }

      int b = bits[i];
      if (b != -1 && isOn){
        data[b / 8] |= 1 << (b % 8);
      }
    }  
  }

  digitalWrite(latchpin, LOW);
  shiftOut(datapin, clockpin, MSBFIRST, ~data[2]);
  shiftOut(datapin, clockpin, MSBFIRST, ~data[1]);
  shiftOut(datapin, clockpin, MSBFIRST, ~data[0]);
  digitalWrite(latchpin, HIGH);
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
  
  output();
  delay(500);
}

