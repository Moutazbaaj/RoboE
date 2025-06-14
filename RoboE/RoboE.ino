#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "driver/dac.h"
#include <math.h> // Required for sin()

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pins
#define SOUND_SENSOR_PIN 34
#define SPEAKER_DAC_PIN 25

// States
enum Mood { HAPPY, ANGRY, SLEEPY };
Mood currentMood = HAPPY;
unsigned long lastChange = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Emotion Bot Starting...");
  
  pinMode(SOUND_SENSOR_PIN, INPUT);
 // dac_output_enable(DAC_CHANNEL_1); // GPIO 25

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found. Check wiring!");
    while (true); // Stop
  }

  display.clearDisplay();
  showMood(currentMood);
}


void loop() {
  int soundLevel = analogRead(SOUND_SENSOR_PIN);
  Serial.print("Sound Level = ");
  Serial.println(soundLevel);

  Mood newMood = currentMood;

  if (soundLevel > 2000) {
    newMood = ANGRY;
  } else if (soundLevel < 500) {
    newMood = SLEEPY;
  } else {
    newMood = HAPPY;
  }

  // Change mood only if different
  if (newMood != currentMood && millis() - lastChange > 2000) {
    lastChange = millis();
    currentMood = newMood;

    Serial.print("Mood changed to: ");
    Serial.println(getMoodName(currentMood));

    showMood(currentMood);
    speakMood(currentMood);
  }

  delay(100);
}

// Show mood on OLED
void showMood(Mood mood) {
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);

  switch (mood) {
    case HAPPY:
      display.print(":)");
      break;
    case ANGRY:
      display.print(">:(");
      break;
    case SLEEPY:
      display.print("-_-");
      break;
  }

  display.display();
}

// Sound output for each mood
void speakMood(Mood mood) {
  int freq = 0;
  switch (mood) {
    case HAPPY:
      freq = 440;
      break;
    case ANGRY:
      freq = 220;
      break;
    case SLEEPY:
      freq = 120;
      break;
  }

  Serial.print("Playing tone: ");
  Serial.println(freq);
  playTone(freq);
}

// Beep using DAC pin
void playTone(int freq) {
  int duration = 300;
  for (int i = 0; i < duration; i++) {
    int val = (sin(2 * PI * freq * i / 10000.0) + 1) * 127;
    dacWrite(SPEAKER_DAC_PIN, val);
    delayMicroseconds(100);
  }
  dacWrite(SPEAKER_DAC_PIN, 0);
}

// Mood name for Serial
const char* getMoodName(Mood mood) {
  switch (mood) {
    case HAPPY: return "Happy";
    case ANGRY: return "Angry";
    case SLEEPY: return "Sleepy";
    default: return "Unknown";
  }
}