//20x4 LCD
#include <LiquidCrystal_I2C.h> //SDA = B7[A4], SCL = B6[A5] STM32/[Arduino]
LiquidCrystal_I2C lcd(0x27, 20, 4);

// _________________________ Motor Declarations

byte x_dpin = 5;
byte x_step_pin = 2;

byte y_dpin = 6;
byte y_step_pin = 3;

byte z_dpin = 7;
byte z_step_pin = 4;

byte cut_motor = 12;
byte STOP = A2;

byte enable_pin = 8;

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


// ___________________________ Menu options _________________________



const int MAX_CUT = 304; //12" in mm
const int MIN_CUT = 90; //Minimum cut length


//Note: if a menu is selected ">" becomes "X".

//Shaft length parameters
int length_step = 1;
long shaft_length = MIN_CUT;
long push_steps;
const int BLADE_DIST = 325;

//Defining pins
//Arduino interrupt pins: 2, 3.
byte RotaryCLK = 13; //CLK pin on the rotary encoder
byte RotaryDT = A3; //DT pin on the rotary encoder
byte button = A0; //Button to enter/exit menu

//__________________Rotary Encoder States

int btn_state;
unsigned long btn_time = millis();
bool pressed = false;

int scroll_state = 0;
int scroll_dir = 1; //1 is Clockwise, -1 is counterclockwise, 0 is nothing
unsigned char scroll_result;
const unsigned char ttable[7][4] = {
  {0, 2, 4, 0},
  {3, 0, 1, 0 | 16},
  {3, 2, 0, 0},
  {3, 2, 1, 0},
  {6, 0, 4, 0},
  {6, 5, 0, 0 | 32},
  {6, 5, 4, 0},
};

int menu_counter = 0; //counts the clicks of the rotary encoder between menu items (0-3 in this case)

bool shaft_menu = false; //enable/disable to change the shaft_lengthue of menu item
bool length_menu = false;
bool home_menu = false;
bool cut_menu = false;
int menu_cursor = 1; //This is the offset of the text, set 0 for shaft setting

//____________________Display Screens___________________
int scroll_min = 0; int scroll_max = 3;
String welcome_screen[4] = {"Shaft Cutter", "Version 2", "by Greg Chris Gabe", "   Gordon"};
String home_screen[4] = {"Set Shaft Length", "Cut Shaft", "Auto Home", "Manual Move"};
String shaft_screen[4] = {"Increment +/- 1", "Increment +/- 10", "Back...", " "};
String length_screen[4] = {"Shaft length [mm]", " ", " ", " "};
String cut_screen[4] = {" ", " ", "Run Cut!", "Back..."};
String screen[4] = home_screen;

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting...");

  //___________________________________ Motor Stuff
  pinMode(cut_motor, OUTPUT);
  digitalWrite(cut_motor, LOW);

  pinMode(enable_pin, OUTPUT);
  digitalWrite(enable_pin, HIGH);

//  pinMode(STOP, INPUT);


  pinMode(x_step_pin, OUTPUT); pinMode(x_dpin, OUTPUT);
  pinMode(y_step_pin, OUTPUT); pinMode(y_dpin, OUTPUT);
  pinMode(z_step_pin, OUTPUT); pinMode(z_dpin, OUTPUT);

  pinMode(xlim_pin, INPUT); pinMode(ylim_pin, INPUT); pinMode(zlim_pin, INPUT);

  //________________________________ Screen Stuff
//  pinMode(RotaryCLK, INPUT); //RotaryCLK
//  pinMode(RotaryDT, INPUT); //RotaryDT
//  pinMode(button, INPUT_PULLUP); //Button

  //------------------------------------------------------
//  lcd.init();                      // initialize the lcd
//  lcd.backlight();
  //------------------------------------------------------
  //  printLCD();
  //
  //  home_menu = true;
  //  printLCD(); //print the stationary parts on the screen
  //  updateCursorPosition();
  Serial.println("setup finished");

}

void loop()
{
  Serial.println("Input shaft length: ");
  handle_input();
}

void handle_input() {
  while (!Serial.available()) {
    //do nothing
  }
  shaft_length = Serial.parseInt();
  run_cut();

}



void update_button() {
  btn_state = digitalRead(button);
  //If we detect LOW signal, button is pressed
  if (btn_state == LOW && (millis() - btn_time > 100)) {
    if (!pressed) {
      select();
      pressed = true;
    }
    btn_time = millis();
  }
  else if (btn_state == HIGH) {
    pressed = false;
  }
}
int rotate() {

  // Grab state of input pins.
  unsigned char pinstate = (digitalRead(RotaryCLK) << 1) | digitalRead(RotaryDT);

  // Determine new state from the pins and state table.
  scroll_state = ttable[scroll_state & 0xf][pinstate];

  // Return emit bits, ie the generated event.
  scroll_result = scroll_state & 48;
  if (scroll_result == 16) {
    return 1;
  }
  else if (scroll_result == 32) {
    return -1;
  }
  else {
    return 0;
  }
}

void scroll() //Triggered whenever the user rotates
{
  if (length_menu == true)
  {
    update_length();
  }
  else
  {
    scroll_menu();
    updateCursorPosition();
  }

}
void scroll_menu() {
  // If the DT state is different than the CLK state then
  // the encoder is rotating in A direction, so we increase
  menu_counter += scroll_dir;
  if (menu_counter < scroll_min) {
    menu_counter = scroll_min;
  }
  else if (menu_counter > scroll_max) {
    menu_counter = scroll_max;
  }
}

int update_length() {
  shaft_length += (scroll_dir * length_step);
  if (shaft_length < MIN_CUT) {
    shaft_length = MIN_CUT;
    lcd.setCursor(0, 2);
    lcd.print("MINIMUM LENGTH");
  }
  else if (shaft_length > MAX_CUT) {
    shaft_length = MAX_CUT;
    lcd.setCursor(0, 2);
    lcd.print("MAXIMUM LENGTH");
  }
  else {
    lcd.setCursor(0, 2);
    lcd.print("                 ");
  }
  lcd.setCursor(0, 1);
  lcd.print("          ");
  lcd.setCursor(0, 1);
  lcd.print(shaft_length);

}

void select()
{
  switch (menu_counter)
  {
    case 0:
      if (length_menu) {
        length_menu = false;
      }
      else if (shaft_menu) {
        length_menu = true;
        length_step = 1;
      }
      else { //home screen
        shaft_menu = true;
      }
      break;

    case 1:
      if (shaft_menu) {
        length_menu = true;
        length_step = 10;
      }
      else {
        cut_menu = true;
      }
      break;

    case 2:
      if (cut_menu) {
        Serial.println("CUT SHAFT!!");
        run_cut();
      }
      else if (shaft_menu) {
        shaft_menu = false;
      }
      else {
        auto_home();
      }
      break;

    case 3:
      if (cut_menu) {
        cut_menu = false;
      }

      break;
  }
  printLCD();
  menu_counter = scroll_min;
  if (!length_menu) {
    updateCursorPosition();
  }
}

void updateCursorPosition()
{
  for (int i = 0; i < 4; i++) {
    lcd.setCursor(0, i);
    if (menu_counter == i) {
      lcd.print(">");
    }
    else {
      lcd.print(" ");
    }
  }
}

void printLCD()
{
  updateLiveMenu();
  lcd.clear(); //clear the whole LCD
  for (int i = 0; i < 4; i++) {
    lcd.setCursor(menu_cursor, i);
    lcd.print(screen[i]);
  }
}
void updateLiveMenu() {
  if (length_menu) {
    for (int i = 0; i < 4; i++) {
      screen[i] = length_screen[i];
    }
    screen[1] = String(shaft_length);
    menu_cursor = 0;
  }
  else if (cut_menu) {
    for (int i = 0; i < 4; i++) {
      screen[i] = cut_screen[i];
    }
    screen[0] = "LENGTH [mm]: " + String(shaft_length);
    menu_cursor = 1;
    scroll_min = 2; scroll_max = 3;
  }
  else if (shaft_menu) {
    for (int i = 0; i < 4; i++) {
      screen[i] = shaft_screen[i];
    }
    menu_cursor = 1;
    scroll_min = 0; scroll_max = 2;
  }
  else if (home_menu) {
    for (int i = 0; i < 4; i++) {
      screen[i] = home_screen[i];
    }
    menu_cursor = 1;
    scroll_min = 0; scroll_max = 3;
  }
  else { //welcome menu
    for (int i = 0; i < 4; i++) {
      screen[i] = welcome_screen[i];
    }
    menu_cursor = 0;
  }
}

void turn_step(int stepPin, int directionPin, bool dir) {
  digitalWrite(directionPin, dir);
  digitalWrite(stepPin, HIGH);
  digitalWrite(stepPin, LOW);
  delay(millisbetweenSteps);
}

void run_cut() {
  auto_home();
  push_steps = long(float(BLADE_DIST - shaft_length) * 1000 / 40);

  digitalWrite(enable_pin, LOW);

  // Pushing first
  Serial.println("Pushing...");
  for (int n = 0; n < push_steps; n++) {
    if (digitalRead(lim_pin[0]) == HIGH) {
      turn_step(x_step_pin, x_dpin, LOW);
    }
    else {
      Serial.println("X limit hit");
      break;
    }
  }

  //Clamp
  Serial.println("Clamping...");
  for (int n = 0; n < 360; n++) {
    turn_step(y_step_pin, y_dpin, HIGH);
    turn_step(z_step_pin, z_dpin, HIGH);
  }

  //Activate cutting motor
  delay(1000);
  Serial.println("CUTTING MOTOR ON");
  digitalWrite(cut_motor, HIGH);
  delay(3000);

  //Cutting
  Serial.println("Cutting...");
  int down_count = 60;
  double del;
  while (down_count < 220) {
    Serial.println("depth: " + String(down_count));
    for (int i = 0; i < down_count; i ++) {
      if (digitalRead(lim_pin[0]) == LOW) {
        Serial.println("interrupted...");
        goto jump;
      }
      turn_step(y_step_pin, y_dpin, HIGH);
      turn_step(z_step_pin, z_dpin, HIGH);
      del = double(i) / double(down_count) * 300;
      delay(int(del));
    }
    for (int i = 0; i < down_count; i ++) {
      if (digitalRead(lim_pin[0]) == LOW) {
        Serial.println("interrupted...");
        goto jump;
      }
      turn_step(y_step_pin, y_dpin, LOW);
      turn_step(z_step_pin, z_dpin, LOW);
    }
    down_count += 10;
  }

jump:
  Serial.println("Cut finished");
  digitalWrite(cut_motor, LOW);
  delay(5000);

  //Homing
  auto_home();
  digitalWrite(enable_pin, HIGH);

}

void auto_home() {
  Serial.println("Auto Homing...");
  digitalWrite(enable_pin, LOW);
  //Homing X first
  while (digitalRead(lim_pin[0]) == HIGH) {
    turn_step(x_step_pin, x_dpin, HIGH);
  }

  //Stepping back off limit switch
  while (digitalRead(lim_pin[0]) == LOW) {
    turn_step(x_step_pin, x_dpin, LOW);
  }

  //Homing Y and Z axes
  while (digitalRead(lim_pin[1]) == HIGH && digitalRead(lim_pin[2]) == HIGH) {
    if (digitalRead(lim_pin[1]) == HIGH) {
      turn_step(y_step_pin, y_dpin, LOW);
    }
    if (digitalRead(lim_pin[2]) == HIGH) {
      turn_step(z_step_pin, z_dpin, LOW);
    }
  }

  delay(500);

  while (digitalRead(lim_pin[1]) == LOW || digitalRead(lim_pin[2]) == LOW) {
    if (digitalRead(lim_pin[1]) == LOW) {
      turn_step(y_step_pin, y_dpin, HIGH);
    }
    if (digitalRead(lim_pin[2]) == LOW) {
      turn_step(z_step_pin, z_dpin, HIGH);
    }
  }

  digitalWrite(enable_pin, HIGH);

}