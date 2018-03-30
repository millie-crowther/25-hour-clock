int datapin = 11;
int clockpin = 12;
int latchpin = 8;

//data for pins
int pin_vals[14];
int pins[14] = {
  6, A5, 2, -1, A4, A3, 3,
  5, 10, A0, 9, A1, A2, 7
};

// data for shift registers
byte data[3];
int bits[] = {
  4, -1, 5, -1, 12, 13, 6,
  15, 11, 2, 8, 0, 7, 14,
  3, 19, 17, -1, 9, 10, 1,
  18, 16, -1, 23, 21, 20, 22
};
  
void setup(){
  Serial.begin(9600);
  for (int i = 2; i < 11; i++){
    if (i != 8){
      pinMode(i, OUTPUT);
    }
  }
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);

  pinMode(datapin, OUTPUT);
  pinMode(clockpin, OUTPUT);
  pinMode(latchpin, OUTPUT);

  for (int i = 0; i < 3; i++){
    data[i] = 0;
  }
}

void print_led(){
  for (int i = 0; i < 14; i++){
    if (pins[i] != -1){
      digitalWrite(pins[i], pin_vals[i]);
    }
  }

  //data = ~data; 
  digitalWrite(latchpin, LOW);
  for (int i = 3; i >= 0; i--){
    shiftOut(datapin, clockpin, MSBFIRST, ~data[i]);
  }
  digitalWrite(latchpin, HIGH);
}

void remember(int digit, int segment, boolean value){
  // smallest second digit
  int pin = -1; 
  int v;
  if (value){
    v = LOW;
  } else {
    v = HIGH;
  }

  // output directly from pins
  
  if (digit == 5 || (digit == 4 && segment != 3)){
    pin = pins[(digit - 4) * 7 + segment];
  }

  if (digit == 3 && segment == 2){
    pin = 4;
  }

  if (pin != -1){
    pin_vals[(digit - 4) * 7 + segment] = v;
    return;
  }

  // output to shift register
  if (digit < 0 || digit > 3){
    return;
  }
  int bit = bits[digit * 7 + segment];
  if (bit == -1){
    return;
  }
  int d = bit / 8;
  int b = bit % 8;
  if (value){
    data[d] |= 1 << b;
  } else {
    data[d] &= ~(1 << b);
  } 
}

void loop(){
  if (Serial.available() > 0){
    byte pi_data = Serial.read();

    int digit = (pi_data & 224) >> 5;
    int segment = (pi_data & 28) >> 2;
    boolean value = pi_data & 1;
    
    if (pi_data & 2){
      print_led();
    } else {
      remember(digit, segment, value);
    }
  }
}

