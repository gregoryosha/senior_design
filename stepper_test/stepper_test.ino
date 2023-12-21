// testing a stepper motor with a Pololu A4988 driver board or equivalent
// on an Uno the onboard led will flash with each step
// this version uses delay() to manage timing

byte x_dpin = 5;
byte x_step_pin = 2;
byte y_dpin = 6;
byte y_step_pin = 3;

int numberOfSteps = 100;
byte ledPin = 13;
int pulseWidthMicros = 2;  // microseconds
int millisbetweenSteps = 2; // milliseconds - or try 1000 for slower steps

// Define the pin that will trigger the interrupt
byte interruptPin = 9;  // Change this to the desired pin number
bool pinState;
bool dir = LOW;
bool stopped = false;


void setup() {

  Serial.begin(9600);
  Serial.println("Starting StepperTest");
  delay(2000);

  pinMode(x_step_pin, OUTPUT); pinMode(x_dpin, OUTPUT);
  pinMode(y_step_pin, OUTPUT); pinMode(y_dpin, OUTPUT);

  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT);

}

void loop() {
  pinState = digitalRead(interruptPin);
  if (pinState == LOW && stopped == false) {
    if (dir == LOW) {
      dir = HIGH;
    }
    else {
      dir = LOW;
    }
    stopped = true;
  }
  
  if (pinState == HIGH) {
    turn_step(y_step_pin, y_dpin, dir);
    stopped = false;
  }

}


void turn_step(int stepPin, int directionPin, bool dir) {
  digitalWrite(directionPin, dir);
  digitalWrite(stepPin, HIGH);
  digitalWrite(stepPin, LOW);
  delay(millisbetweenSteps);

}
