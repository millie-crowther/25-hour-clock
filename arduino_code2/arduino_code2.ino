int datapin  = 5;
int clockpin = 2;
int latchpin = 3;

//data for pins
int pins[42] = {
  -1, -1, -1, -1, -1, -1, -1, // 0
  -1, -1, -1, -1, -1, -1, -1, // 1
  A5, -1, -1, -1, -1,  4, -1, // 2
  11, A2, A3, A4, A1, -1, A0, // 3
  10, -1, 12, -1, 13, -1, -1, // 4
  -1, -1,  6, -1,  8,  9, -1  // 5
};

// data for shift registers
byte data[3];
int bits[42] = {
  -1, 12,  3, 13,  4, 14, -1, // 0
   2,  6, 15,  5,  8, 10,  0, // 1
  -1,  9,  1, 11,  7, -1, -1, // 2
  20, -1, -1, -1, -1, 19, -1, // 3
  -1, 18, -1, 17, -1, 16, -1, // 4
  -1, 22, -1, 23, -1, -1, 21  // 5
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
  int t = current_time();
  int h = (t / 3600) % 25;
  int m = (t / 60) % 60;
  int s = t % 60;

  int digits[6] {
    h / 10, h % 10,
    m / 10, m % 10,
    s / 10, s % 10
  };

  // do shift register output
  for (int i = 0; i < 3; i++){
    data[i] = 0;
  }

  for (int i = 0; i < 42; i++){
    int digit = i / 7;
    int segment = i % 7;
    
    boolean isOn = get_value(digits[digit], segment);
    
    if (pins[i] != -1){
      digitalWrite(i, isOn ? LOW : HIGH);
    }

    int b = bits[i];
    if (b != -1 && isOn){
      data[b / 8] |= 1 << (b % 8);
    }  
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
  
  output();
  delay(200);
}

