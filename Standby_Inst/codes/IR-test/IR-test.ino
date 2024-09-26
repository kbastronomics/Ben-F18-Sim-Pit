// IR sensor Test
// http://www.electronoobs.com/eng_arduino_tut74.php

int LED = 9;
int LED_TEST = 8;
int ROLL_ZERO_IRPin = 22;
int WL_ZERO_IRPin = 23;
int HOZ_ZERO_IRPin = 24;
int VER_ZERO_IRPin = 25;

int ROLL_ZERO_IR;// = HIGH;
int WL_ZERO_IR;// = LOW;
int HOZ_ZERO_IR;// = LOW;
int VER_ZERO_IR;// = LOW;

void setup() {

  pinMode (LED, OUTPUT);
 pinMode(ROLL_ZERO_IRPin, INPUT);
   pinMode(WL_ZERO_IRPin, INPUT);
 pinMode(HOZ_ZERO_IRPin, INPUT);
    pinMode(VER_ZERO_IRPin, INPUT);

  Serial.begin(9600);

}

void loop() {


 // int Value22 = digitalRead(22);
 // if (digitalRead(25) == HIGH) {
 //   digitalWrite(LED, HIGH);
 // }
 // else {
 //   digitalWrite(LED, LOW);
 // }

digitalWrite(LED_TEST, HIGH);
 
int Value22 = digitalRead(22);
Serial.println("22 - WL");
Serial.print("Digital read: ");Serial.println(Value22);
 Serial.println("");
    
  int Value23 = digitalRead(23);
   Serial.println("23 - ROLL");
   Serial.print("Digital read: ");Serial.println(Value23);
 Serial.println("");


int Value24 = digitalRead(24);
Serial.println("24 HOZ");
Serial.print("Digital read: ");Serial.println(Value24);
 Serial.println("");


int Value25 = digitalRead(25);
Serial.println("25 VER");
Serial.print("Digital read: ");Serial.println(Value25);
 Serial.println("");
  delay(5000);
   Serial.println("");
  Serial.println("");
}
