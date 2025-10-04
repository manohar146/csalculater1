#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// LCD Setup: 20x4 I2C (Try address 0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Change to 0x3F if nothing appears

// Keypad Layout
char keys[4][4] = {
  {'1', '2', '3', '+'},
  {'4', '5', '6', '-'},
  {'7', '8', '9', '*'},
  {'C', '0', '=', '/'}
};

// Safe GPIO Pins for ESP32
byte rowPins[4] = {13, 12, 14, 25};  // R1-R4
byte colPins[4] = {18, 5, 4, 15};    // C1-C4

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);

#define BUZZER_PIN 27
#define LED_PIN 26

String input = "";
char operatorChar = '\0';

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calculator Ready");
  Serial.println("🔢 Calculator Ready. Use keypad.");
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    // Feedback
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    // Clear
    if (key == 'C') {
      input = "";
      operatorChar = '\0';
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Cleared");
      lcd.setCursor(0, 1);
      lcd.print("Input: ");
      Serial.println("🧹 Cleared");
      return;
    }

    // Evaluate
    else if (key == '=') {
      lcd.setCursor(0, 1);
      lcd.print("Input: " + input + "    ");
      Serial.println("Input: " + input);
      if (operatorChar != '\0') {
        int opIndex = input.indexOf(operatorChar);
        if (opIndex > 0 && opIndex < input.length() - 1) {
          float num1 = input.substring(0, opIndex).toFloat();
          float num2 = input.substring(opIndex + 1).toFloat();
          float result = 0;
          bool valid = true;

          switch (operatorChar) {
            case '+': result = num1 + num2; break;
            case '-': result = num1 - num2; break;
            case '*': result = num1 * num2; break;
            case '/':
              if (num2 != 0) result = num1 / num2;
              else {
                lcd.setCursor(0, 2);
                lcd.print("  Divide by 0     ");
                Serial.println("❌ Error: Divide by 0");
                valid = false;
              }
              break;
          }

          if (valid) {
            lcd.setCursor(0, 2);
            lcd.print("Result: ");
            lcd.print(result);
            Serial.print("✅ Result: ");
            Serial.println(result);
          }
        } else {
          lcd.setCursor(0, 2);
          lcd.print("⚠ Format error     ");
          Serial.println("⚠️ Format error. Try 7*8=");
        }
      } else {
        lcd.setCursor(0, 2);
        lcd.print("⚠ No operator!     ");
        Serial.println("⚠️ No operator found!");
      }

      input = "";
      operatorChar = '\0';
      // ❌ Removed delay + clear, keep result until next keypress
    }

    // Input keys
    else {
      if (key == '+' || key == '-' || key == '*' || key == '/') {
        if (operatorChar == '\0') {
          operatorChar = key;
          input += key;
        } else {
          lcd.setCursor(0, 3);
          lcd.print("⚠ 1 operator only ");
          Serial.println("⚠️ Operator already used.");
          delay(1500);
          lcd.setCursor(0, 3);
          lcd.print("                    ");
        }
      } else if (key >= '0' && key <= '9') {
        input += key;
      }

      // Display current input
      lcd.setCursor(0, 0);
      lcd.print("You pressed: ");
      lcd.print(key);
      lcd.print("     "); // padding
      lcd.setCursor(0, 1);
      lcd.print("Input: " + input + "     ");
      Serial.print("You pressed key: ");
      Serial.println(key);
      Serial.println("Input: " + input);
    }
  }
}
