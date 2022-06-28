#include "Arduino.h"


#define PUSHED LOW
#define UNPUSHED HIGH


const byte led = 14;
const byte ledblue = 16;
const byte btn = 4;

byte ledState = HIGH;
byte ButtonState;
byte lastButtonState = UNPUSHED; // the previous reading from the input pin
int pushcounter = 0;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup()
{
  Serial1.begin(115200); // Start the Serial communication to send messages to the computer
  //Serial.swap();
  delay(10);


  pinMode(led, OUTPUT);
  pinMode(ledblue, OUTPUT);
  pinMode(btn, INPUT);

  digitalWrite(led, ledState);
}

unsigned long previousTime = millis();
const unsigned long interval = 500;

void loop()
{
  digitalWrite(ledblue, HIGH);

byte reading = digitalRead(btn);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  digitalWrite(ledblue, LOW);
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != ButtonState) {
      ButtonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (ButtonState == HIGH) {
        ledState = !ledState;
      }
    }
  }

  // set the LED:
  digitalWrite(led, ledState);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;


  //delay(100);


}
