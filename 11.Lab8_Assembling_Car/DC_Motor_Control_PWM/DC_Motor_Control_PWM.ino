// Motor A
int motor1Pina = 12;
int motor1Pinb = 13;
 
int motor2Pina = 14; 
int motor2Pinb = 15; 

void setup() {
  // sets the pins as outputs:
  pinMode(motor1Pina, OUTPUT);
  pinMode(motor1Pinb, OUTPUT);
  pinMode(motor2Pina, OUTPUT);
  pinMode(motor2Pinb, OUTPUT);
  Serial.begin(115200);

  // testing
  Serial.print("Testing DC Motor...");
}

void loop() {
  static int i=0;
  for(i=0;i<100;i++){
  // Move the Both DC motors Backward 
  digitalWrite(motor1Pina, HIGH);
  digitalWrite(motor1Pinb, LOW); 
  digitalWrite(motor2Pina, LOW);
  digitalWrite(motor2Pinb, HIGH); 
  delay(10);
  digitalWrite(motor1Pina, LOW);
  digitalWrite(motor1Pinb, LOW); 
  digitalWrite(motor2Pina, LOW);
  digitalWrite(motor2Pinb, LOW); 
  delay(10);
  }
  for(i=0;i<100;i++){
  // Move the Both DC motors Backward 
  digitalWrite(motor1Pina, LOW);
  digitalWrite(motor1Pinb, HIGH); 
  digitalWrite(motor2Pina, HIGH);
  digitalWrite(motor2Pinb, LOW); 
  delay(10);
  digitalWrite(motor1Pina, LOW);
  digitalWrite(motor1Pinb, LOW); 
  digitalWrite(motor2Pina, LOW);
  digitalWrite(motor2Pinb, LOW); 
  delay(10);
  }
  
}
