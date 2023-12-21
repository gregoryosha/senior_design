byte x_dpin = 5;
byte x_step_pin = 2;

byte y_dpin = 6;
byte y_step_pin = 3;

byte z_dpin = 7;
byte z_step_pin = 4;

byte cut_motor = 12;
byte STOP = A2;

int pulseWidthMicros = 2;  // microseconds
int millisbetweenSteps = 3; // milliseconds - or try 1000 for slower steps

// Define the pin that will trigger the interrupt
byte xlim_pin = 9;  // Change this to the desired pin number
byte ylim_pin = 10;
byte zlim_pin = 11;
byte lim_pin[3] = {9, 10, 11}; //x, y, z
bool lim_state[3] = {HIGH, HIGH, HIGH};
bool motor_state[3] = {LOW, LOW, LOW};

long dist = 0;
bool dir = LOW;
bool stopped = false;

long steps = 0;

byte enable_pin = 8;


void setup() {
  Serial.begin(9600);
  Serial.println("Starting StepperTest");
  delay(2000);
  pinMode(enable_pin, OUTPUT);
  digitalWrite(enable_pin, LOW);

  pinMode(cut_motor, OUTPUT);
  digitalWrite(cut_motor, LOW);

  pinMode(STOP, INPUT);


  pinMode(x_step_pin, OUTPUT); pinMode(x_dpin, OUTPUT);
  pinMode(y_step_pin, OUTPUT); pinMode(y_dpin, OUTPUT);
  pinMode(z_step_pin, OUTPUT); pinMode(z_dpin, OUTPUT);

  pinMode(xlim_pin, INPUT); pinMode(ylim_pin, INPUT); pinMode(zlim_pin, INPUT);

}

void loop() {
  Serial.println("Pushing distance in mm: ");
  handle_input();

  motor_state[0] = HIGH;

  for (int n = 0; n < steps; n++) {
    //check_stop();
    if (digitalRead(lim_pin[0]) == LOW) {
      motor_state[0] = LOW;
      Serial.println("limit switch hit");
    }
    if (motor_state[0]) {
      turn_step(x_step_pin, x_dpin, dir);
    }
    else {
      break;
    }
  }

  Serial.println("Clamp piece (-10mm): ");
  handle_input();
  motor_state[1] = HIGH;
  motor_state[2] = HIGH;

  for (int n = 0; n < steps; n++) {
    //check_stop();
    turn_step(y_step_pin, y_dpin, dir);
    turn_step(z_step_pin, z_dpin, dir);
    delay(20);
  }

  //__________________________________________
  Serial.println("Run cutting motor? [y/n]");
  while (!Serial.available()) {
    check_stop();
  }
  char choice = Serial.read();
  if (choice == 'y') {
    digitalWrite(cut_motor, HIGH);
  }
  else {
    digitalWrite(cut_motor, LOW);
  }

  //___________________
  Serial.println("Raise distance in mm (negative is down): ");
  handle_input();
  motor_state[1] = HIGH;
  motor_state[2] = HIGH;

  for (int n = 0; n < steps-40; n++) {
    check_stop();
    for (int i = 0; i < 40; i ++) {
      turn_step(y_step_pin, y_dpin, dir);
      turn_step(z_step_pin, z_dpin, dir);
      delay(50);
      n++;
    }
    for (int i = 0; i < 20; i ++) {
      turn_step(y_step_pin, y_dpin, !dir);
      turn_step(z_step_pin, z_dpin, !dir);
      delay(50);
      n--;
    }
    
  }
  //  for (int i = 0; i < 3; i++) {
  //    lim_state[i] = digitalRead(lim_pin[i]);
  //    if (lim_state[i] == LOW) {
  //      motor_state[i] = LOW;
  //    }
  //  }


}
void check_stop() {
  //  Serial.println("stopping motor...");
  //  if (digitalRead(STOP) == HIGH) {
  //    digitalWrite(cut_motor, LOW);
  //  }
}
void handle_input() {
  while (!Serial.available()) {
    //do nothing
  }
  if (Serial.available() > 0) {
    dist = Serial.parseInt();
    steps = int(float(dist) * 1000 / 38.5);

    if (steps < 0) {
      dir = HIGH;
      steps = -1 * steps;
    }
    else {
      dir = LOW;
    }
    if (dist == 69) {
      Serial.println("Auto Homing...");
      steps = 0;
      auto_home();
    }
  }
}

void turn_step(int stepPin, int directionPin, bool dir) {
  digitalWrite(directionPin, dir);
  digitalWrite(stepPin, HIGH);
  digitalWrite(stepPin, LOW);
  delay(millisbetweenSteps);
}
void auto_home() {

  //Homing X first
  while (digitalRead(lim_pin[0]) == HIGH) {
    turn_step(x_step_pin, x_dpin, HIGH);
  }
  //Stepping back off limit switch
  for (int n = 0; n < 100; n++) {
    turn_step(x_step_pin, x_dpin, LOW);
  }

  //Homing Y and Z axes
  motor_state[1] = HIGH;
  motor_state[2] = HIGH;

  while (true) {
    if (digitalRead(lim_pin[1]) == HIGH) {
      turn_step(y_step_pin, y_dpin, LOW);
    }
    else {
      motor_state[1] = LOW;
    }

    if (digitalRead(lim_pin[2]) == HIGH) {
      turn_step(z_step_pin, z_dpin, LOW);
    }
    else {
      motor_state[2] = LOW;
    }
    if (motor_state[1] == LOW && motor_state[2] == LOW) {
      break;
    }
  }
  delay(500);

  //Stepping back off limit switch
  for (int n = 0; n < 100; n++) {
    turn_step(y_step_pin, y_dpin, HIGH);
    turn_step(z_step_pin, z_dpin, HIGH);
  }


}