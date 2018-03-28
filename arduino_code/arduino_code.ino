int my_digit = -1;

void setup(){
  Serial.begin(9600);
}

void output(int digit, int segment, boolean value){
  if (digit != my_digit && digit != my_digit + 1){
    //data erroneously sent here
    return;
  }

  int pin = (digit - my_digit) * 7 + segment;

  if (value){
    digitalWrite(pin, HIGH);
  } else {
    digitalWrite(pin, LOW);
  }
}

void loop(){
  if (Serial.available() > 0){
    byte data = Serial.read();

    int digit = (data & 224) >> 5;
    int segment = (data & 28) >> 2;
    boolean value = data & 1;
    
    if (data & 2){
      // set digits that this arduino handles
      my_digit = digit;
    } else {
      // output digit
      output(digit, segment, value);
    }
  }
}

