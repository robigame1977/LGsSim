#include <LiquidCrystal.h>

// Pin definitions
const int ledPin = 11;
const int inPin = 12;   // Left button
const int inPin2 = 13;  // Right button
const int confPin = 4;  // Config button (SHIFT)

// LCD initialization
LiquidCrystal lcd(2, 3, 6, 7, 8, 9);

// Global variables
bool menuSelect = false;
int menuPage = 0;
const int totalPages = 3; // Total number of menu pages
int selectedMode = 0; // Tracks the selected mode
unsigned long lastElapsedUpdate = 0;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Hello, World!");

  pinMode(ledPin, OUTPUT);
  pinMode(inPin, INPUT);
  pinMode(inPin2, INPUT);
  pinMode(confPin, INPUT);

  // Initial loader
  displayLoader();
}

void loop() {
  handleMenu();
  updateElapsed();
  simulateLogic();
}

// Handle menu navigation and selection
void handleMenu() {
  static int lastConfigState = LOW;
  static int lastLeftState = LOW;
  static int lastRightState = LOW;

  int configState = digitalRead(confPin);
  int leftState = digitalRead(inPin);
  int rightState = digitalRead(inPin2);

  // Toggle menu select mode on config button press (when menu is OFF)
  if (configState == HIGH && lastConfigState == LOW && !menuSelect) {
    menuSelect = true;
    menuPage = 0;
    lcd.clear();
    displayMenu();
  }
  
  if (menuSelect) {
    if (configState == LOW) {
      // Page navigation when config is not held
      if (leftState == HIGH && lastLeftState == LOW) {
        menuPage = (menuPage - 1 + totalPages) % totalPages;
        displayMenu();
        delay(200); // Simple debounce
      }
      if (rightState == HIGH && lastRightState == LOW) {
        menuPage = (menuPage + 1) % totalPages;
        displayMenu();
        delay(200); // Simple debounce
      }
    } else {
      // Mode selection when config is held
      if (leftState == HIGH && lastLeftState == LOW) {
        selectedMode = menuPage * 2; // Select left mode
        applySelection();
        delay(200); // Simple debounce
      }
      if (rightState == HIGH && lastRightState == LOW) {
        selectedMode = menuPage * 2 + 1; // Select right mode
        applySelection();
        delay(200); // Simple debounce
      }
    }
  }

  lastConfigState = configState;
  lastLeftState = leftState;
  lastRightState = rightState;
}

// Display menu content based on page
void displayMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select Mode:");

  if (menuPage == 0) {
    lcd.setCursor(0, 1);
    lcd.print("1. AND    2. OR");
  } else if (menuPage == 1) {
    lcd.setCursor(0, 1);
    lcd.print("3. XOR    4. NOR");
  } else if (menuPage == 2) {
    lcd.setCursor(0, 1);
    lcd.print("5. NAND   6. XNOR");
  }

  lcd.setCursor(14, 0);
  lcd.print(menuPage + 1);
  lcd.print("/");
  lcd.print(totalPages);
}

// Apply selected mode logic
void applySelection() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode Selected:");
  lcd.setCursor(0, 1);
  lcd.print("Mode ");
  lcd.print(selectedMode + 1); // Display selected mode (1-based index)
  delay(1000);
  menuSelect = false; // Exit menu
  lcd.clear();
}

// Update elapsed time on the main screen
void updateElapsed() {
  if (!menuSelect && millis() - lastElapsedUpdate > 500) {
    lastElapsedUpdate = millis();
    lcd.setCursor(0, 0);
    lcd.print("LGs Simulator M" + String(selectedMode + 1));
    lcd.setCursor(0, 1);
    lcd.print("Elapsed: ");
    lcd.print(millis() / 1000);
    lcd.print(" s");
  }
}

// Simulate logic gate behavior
void simulateLogic() {
  if (!menuSelect) {
    bool i1 = digitalRead(inPin);
    bool i2 = digitalRead(inPin2);

    if (selectedMode == 0 && (i1 && i2)) { // AND gate
      digitalWrite(ledPin, HIGH);
    } else if (selectedMode == 1 && (i1 || i2)) { // OR gate
      digitalWrite(ledPin, HIGH);
    } else if (selectedMode == 2 && XOR(i1, i2)) { // XOR gate
      digitalWrite(ledPin, HIGH);
    } else if (selectedMode == 3 && !(i1 || i2)) { // NOR gate
      digitalWrite(ledPin, HIGH);
    } else if (selectedMode == 4 && !(i1 && i2)) { // NAND gate
      digitalWrite(ledPin, HIGH);
    } else if (selectedMode == 5 && !XOR(i1, i2)) { // XNOR gate
      digitalWrite(ledPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
    }
  }
}

// Display a simple loader animation
void displayLoader() {
  lcd.clear();
  lcd.print("Loading... v1.31");
  for (int i = 0; i <= 16; i++) {
    lcd.setCursor(i, 1);
    lcd.write(byte(255));
    delay(100);
  }
  lcd.clear();
}

// XOR logic
bool XOR(bool a, bool b) {
  return a != b;
}
