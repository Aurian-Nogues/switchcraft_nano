#include <Arduino.h>

// Pin D2 is relay IN1 -> LCD panel circuit
// Pin D3 is relay IN2 -> Raspberry pi circuit
// LOW closes the circuit and enables power
// PI GPIO 20 is OUTPUT, Nano D5 is corresponding INPUT


int LCD_CIRCUIT_PIN = 2 ;
int PI_CIRCUIT_PIN = 3 ;
int BUTTON_PIN = 4;
int PI_LISTEN_PIN = 5;
int PI_SEND_PIN = 6;
int PI_ON_SIGNAL_CHECK = 7;

bool SYSTEM_ON = false;

void bootSequence(){
  Serial.println("Initiating boot sequence");
  // send signal that Pi should be on
  digitalWrite(PI_SEND_PIN, HIGH);
  // close Pi circuit (ON)
  digitalWrite(PI_CIRCUIT_PIN, LOW);
  
  delay(6000);
  // close LCD panel circuit (ON)
  digitalWrite(LCD_CIRCUIT_PIN, LOW);
  SYSTEM_ON = true;
  // let the pay time to start so button doesn't get spammed
  delay(10000); 
}

void shutDownSequence(){
    Serial.println("Initiating poweroff sequence");

  // send signal to Pi to shutdown
  digitalWrite(PI_SEND_PIN, LOW);

  // open LCD circuit (off)
  digitalWrite(LCD_CIRCUIT_PIN, HIGH);
  delay(4000);
  // open Pi circuit (off)
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
    // for circuit pins, LOW means circuit is closed (on), HIGH means open (off)
    pinMode(LCD_CIRCUIT_PIN, OUTPUT);
    pinMode(PI_CIRCUIT_PIN, OUTPUT);
    // button press will send LOW signal, HIGH is default unpressed
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    // PI will send a LOW signal when ON, this is to avoid white noise when Pi is off
    pinMode(PI_ON_SIGNAL_CHECK, INPUT_PULLUP);
    
    //PI will send LOW to request a shutdown, will send high otherwise
    pinMode(PI_LISTEN_PIN, INPUT_PULLUP); 
    // PI is expecting a HIGH signal at all times, will take LOW as shutdown signal
    pinMode(PI_SEND_PIN, OUTPUT);


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

  if(digitalRead(PI_LISTEN_PIN) == LOW && digitalRead(PI_ON_SIGNAL_CHECK) == LOW) {
    shutDownSequence();
  }

}
