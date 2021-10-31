#include <Arduino.h>

// Pin D2 is relay IN1 -> LCD panel circuit
// Pin D3 is relay IN2 -> Raspberry pi circuit
// LOW closes the circuit and enables power

int LCD_CIRCUIT_PIN = 2 ;
int PI_CIRCUIT_PIN = 3 ;
int BUTTON_PIN = 4;

bool SYSTEM_ON = false;

void bootSequence(){
  digitalWrite(PI_CIRCUIT_PIN, LOW);
  delay(1000);
  digitalWrite(LCD_CIRCUIT_PIN, LOW);
  SYSTEM_ON = true;
}

void shutDownSequence(){
  digitalWrite(LCD_CIRCUIT_PIN, HIGH);
  delay(2000);
  digitalWrite(PI_CIRCUIT_PIN, HIGH);
  SYSTEM_ON = false;

}

bool debounce_button() {
  static uint16_t state = 0;
  state = (state<<1) | digitalRead(BUTTON_PIN) | 0xfe00;
  return (state == 0xff00);
}

void setup() {
    Serial.begin(9600);

    // Assign pins
    pinMode(LCD_CIRCUIT_PIN, OUTPUT);
    pinMode(PI_CIRCUIT_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // start with all circuits closed
    digitalWrite(LCD_CIRCUIT_PIN, HIGH);
    digitalWrite(PI_CIRCUIT_PIN, HIGH);

    //start pi
    bootSequence();
    

}

void loop() {

  
  if (debounce_button()) {
    Serial.println("Button press");
    if (SYSTEM_ON) {
      shutDownSequence();
    } else {
      bootSequence();
    }
  }
}

