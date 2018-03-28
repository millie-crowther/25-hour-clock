void setup(){
  Serial.begin(9600);
}

void output(int digit, int segment, boolean value){
   Serial.print(digit);
   Serial.print(segment);
   Serial.println(value);
}

void loop(){
  if (Serial.available() > 0){
    byte data = Serial.read();

    int digit = (data & 224) >> 5;
    int segment = (data & 28) >> 2;
    boolean value = data & 1;

    output(digit, segment, value);
  }
}

