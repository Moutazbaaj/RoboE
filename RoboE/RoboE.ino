#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "driver/dac.h"
#include "driver/i2s.h"
#include <math.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SPEAKER_DAC_PIN 25

// I2S Microphone config (INMP441)
#define I2S_WS   15  // LRCL
#define I2S_SD   32  // DOUT
#define I2S_SCK  14  // BCLK

#define SAMPLE_BUFFER_SIZE 1024

// States
enum Mood { HAPPY, ANGRY, SLEEPY };
Mood currentMood = HAPPY;
unsigned long lastChange = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Emotion Bot with INMP441 Starting...");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found!");
    while (1);
  }

  display.setRotation(1);
  display.clearDisplay();
  showMood(currentMood);

  setupI2SMic();
}
void loop() {
  int32_t samples[SAMPLE_BUFFER_SIZE];
  size_t bytesRead;

  i2s_read(I2S_NUM_0, (void*)samples, sizeof(samples), &bytesRead, portMAX_DELAY);
  int sampleCount = bytesRead / sizeof(int32_t);

  // Normalize and compute average absolute amplitude
  uint64_t sum = 0;
  for (int i = 0; i < sampleCount; i++) {
    int32_t sample = samples[i] >> 14;  // downscale to 18-bit typical range
    sum += abs(sample);
  }
  uint32_t avg = sum / sampleCount;

  // Convert to dB for more intuitive thresholding (optional)
  float dB = 20.0 * log10((float)avg + 1); // avoid log(0)
  Serial.printf("Avg Amplitude: %lu → %.2f dB\n", avg, dB);

  Mood newMood = currentMood;

  // Thresholds based on dB (you can tweak these)
  if (dB > 70.0) {
    newMood = ANGRY;
  } else if (dB < 30.0) {
    newMood = SLEEPY;
  } else {
    newMood = HAPPY;
  }

  // Change mood only if different and 2s has passed
  if (newMood != currentMood && millis() - lastChange > 2000) {
    lastChange = millis();
    currentMood = newMood;

    Serial.print("Mood changed to: ");
    Serial.println(getMoodName(currentMood));

    showMood(currentMood);
    speakMood(currentMood);
  }

  delay(500);
}

// I2S Setup for INMP441
void setupI2SMic() {
  const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 256,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  i2s_zero_dma_buffer(I2S_NUM_0);
}

// Mood display
void showMood(Mood mood) {
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 55);

  switch (mood) {
    case HAPPY:  display.print(":)"); break;
    case ANGRY:  display.print(">:("); break;
    case SLEEPY: display.print("-_-"); break;
  }

  display.display();
}

// Sound output
void speakMood(Mood mood) {
  int freq = 0;
  switch (mood) {
    case HAPPY: freq = 440; break;
    case ANGRY: freq = 220; break;
    case SLEEPY: freq = 120; break;
  }

  Serial.print("Playing tone: ");
  Serial.println(freq);
  playTone(freq);
}

void playTone(int freq) {
  int duration = 300;
  for (int i = 0; i < duration; i++) {
    int val = (sin(2 * PI * freq * i / 10000.0) + 1) * 127;
    dacWrite(SPEAKER_DAC_PIN, val);
    delayMicroseconds(100);
  }
  dacWrite(SPEAKER_DAC_PIN, 0);
}

const char* getMoodName(Mood mood) {
  switch (mood) {
    case HAPPY: return "Happy";
    case ANGRY: return "Angry";
    case SLEEPY: return "Sleepy";
    default: return "Unknown";
  }
}