int datapin = 11;
int clockpin = 12;
int latchpin = 8;

//data for pins
int digital_vals[14];
int analogue_vals[6];
int pins[14] = {
  5, 2, 3, -1, A5, A2, 4,
  13, 6, A4, 9, A0, A1, 10
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
  pinMode(13, OUTPUT);

  for (int i = 0; i < 3; i++){
    data[i] = 0;
  }
}

void print_led(){
  for (int i = 2; i < 14; i++){
    if (i != 8 && i != 11 && i != 12){
      digitalWrite(i, digital_vals[i]);
    }
  }

  for (int i = 0; i < 6; i++){
    digitalWrite(i + A0, analogue_vals[i]);
  }

  //data = ~data; 
  digitalWrite(latchpin, LOW);
  for (int i = 2; i >= 0; i--){
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
  
  if (digit == 5 || digit == 4){
    pin = pins[(digit - 4) * 7 + segment];
  }

  if (digit == 3){
    if (segment == 1){
      pin = A3;
    }
    if (segment == 2){
      pin = 7;
    }
  }

  if (pin != -1){
    if (pin > 2 && pin < 14){
      digital_vals[pin] = v;
    } else {
      analogue_pins[pin - A0] = v;
    }
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
   /* 
    for (int i = 2; i < 14; i++){
      if (i != 8 && i != 11 & i != 12){
        digitalWrite(i, LOW);
      }
    }

    for (int i = A0; i <= A5; i++){
      digitalWrite(i, LOW);
    }*/

    if (pi_data & 2){
      print_led();
    } else {
      /*Serial.print(digit);
      Serial.print(segment);
      Serial.println(value);*/
      remember(digit, segment, value);
    }
  }
}

