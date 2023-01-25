/*
 * Current Working Version
 */

#include <Adafruit_NeoPixel.h>
#include <Adafruit_TiCoServo.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define SERVO_PIN      10
#define LIGHTS_PIN     9
#define MOTION_PIN     3
#define NUMPIXELS      151 // 91+48
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, LIGHTS_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_TiCoServo myservo;                                   

int LEDstate = HIGH; //default to off
int reading;
int previous = LOW;//default to off

int r = 226, g = 105, b = 10, brightness = 128;

//unsigned means it must be positive, not negative
unsigned long time = 0;
unsigned long debounce = 10;

//changes the entire strip to one solid color
void setColor (int r, int g, int b, int brightness) {
  strip.setBrightness (brightness);
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
    delay(0);
  }
  strip.show();
  return;
}

void fluctuate (int red, int green, int blue, int bright) { //26, 5, 0
  float brightness;

  for (int k = 0; k < 256; k += 4) {
    brightness = (k / 256.0) * bright;
    setColor((int) red, (int)green, (int)blue, (int)brightness);
    strip.show();
    if (readSensor()) {
      return;
    }
  }

  for (int k = 255; k >= 0; k -= 4) {
    brightness = (k / 256.0) * bright;
    setColor((int)red, (int)green, (int)blue, (int)brightness);
    strip.show();
    if (readSensor()) {
      return;
    }
  }
}

void createFlame (int r, int g, int b) {
  for (int i = 0; i < strip.numPixels(); i++) {
    int flicker = random(0, 55);
    int r1 = r - flicker;
    int g1 = g - flicker;
    int b1 = b - flicker;
    if (g1 < 0) g1 = 0;
    if (r1 < 0) r1 = 0;
    if (b1 < 0) b1 = 0;
    strip.setPixelColor(i, r1, g1, b1);
  }
  strip.show();

  //  Adjust the delay here, if you'd like.  Right now, it randomizes the
  //  color switch delay to give a sense of realism
  delay(random(10, 113));
}

//returns true if switch is up, false if switch is down
boolean readSensor () {
  int switchstate;

  reading = digitalRead(MOTION_PIN);

  // If the switch changed, due to bounce or pressing...
  if (reading != previous) {
    // reset the debouncing timer
    time = millis();
  }

  if ((millis() - time) > debounce) {
    // whatever the switch is at, its been there for a long time
    // so lets settle on it!
    switchstate = reading;

    // Now invert the output on the pin13 LED
    if (switchstate == HIGH)
      LEDstate = LOW;
    else
      LEDstate = HIGH;
  }
  digitalWrite(LIGHTS_PIN, LEDstate);

  // Save the last reading so we keep a running tally
  previous = reading;
  if (LEDstate == HIGH) {
    return true;
  }
  else return false;
}

//steadily increases the brightness of the lights
void rampUp() {
  for (int i = 50; i > 0; i--) {
    createFlame(r, g - i, b - i / 5);
    if (!readSensor()) {
      return;
    }
    delay(20);
  }
}


void setup() {
  // put your setup code here, to run once:
  pinMode (MOTION_PIN, INPUT);
  digitalWrite(MOTION_PIN, HIGH);
  pinMode (LIGHTS_PIN, OUTPUT);
  strip.begin();
  strip.setBrightness(brightness);
  strip.show();
  myservo.attach(SERVO_PIN);
  myservo.write(90);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!readSensor()) {
    //dull orange
    myservo.write(180);
    fluctuate(50, 12, 0, brightness);
  }
  else {
    //gradually increases the color to full
    myservo.write(80);
    rampUp();
    while (readSensor()) {
      //  Flicker, based on our initial RGB values
      createFlame (r, g, b);
      //Lightning Effect
      if (random(0, 100) >= 99) {
        setColor (100, 100, 255, brightness);
      }
    }
  }
}
