 """# 🤖 Emotion Bot – ESP32 Sound Reactive OLED + LED + DAC Project

The **Emotion Bot** is a fun and expressive ESP32-powered robot that reacts to ambient sound using:
- 🎤 An **INMP441 I2S microphone** to measure sound level (dB)
- 🖥 A **0.96" OLED screen** to show emotional states
- 🌈 An **addressable LED strip (WS2812B)** to reflect mood with smooth color transitions

---

## 🎯 Features

| Feature            | Description |
|--------------------|-------------|
| 🔊 Sound sensing   | Real-time audio analysis using I2S microphone (INMP441) |
| 🎭 Mood detection  | Converts dB values into emotional states |
| 🌈 LED expression  | LED strip changes color from **white → green → red** based on loudness |
| 🖼 OLED feedback   | Displays animated emotional faces (sleepy, angry, happy, etc.) |

---

## 📊 Mood Mapping by Sound Level (dB)

| dB Range | Mood         | Description                                  | LED Color      |
|----------|--------------|----------------------------------------------|----------------|
| < 20     | `SLEEPY`     | Super quiet, maybe no one’s here             | White          |
| 20–30    | `DISAPPOINTED` | Dead room vibe                              | Pale white     |
| 30–40    | `CONFOUNDED` | Confusingly quiet                            | Cool green     |
| 40–50    | `CONFUSED`   | Low chatter or hum                           | Light green    |
| 50–60    | `GRINNING`   | People talking, warming up                   | Lime green     |
| 60–70    | `KISSING`    | Romantic or soft music atmosphere            | Yellow         |
| 70–80    | `HAPPY`      | Peak vibes, ideal party zone                 | Orange         |
| ≥ 80     | `ANGRY+`     | Too loud – starts turning RED progressively  |  Red tones   |

---

##  Components Used

- [x] ESP32 Mini D1 (or compatible)
- [x] INMP441 I2S Microphone
- [x] 0.96" OLED (SSD1306, I2C)
- [x] WS2812B LED Strip (NeoPixel)
- [x] Optional: Pull-up resistors for I2C

---

## 🔧 Setup & Libraries

Install these libraries from Arduino Library Manager:

```cpp
FastLED
Adafruit_SSD1306
Adafruit_GFX
I2S
Wire
```

Configure I2C and I2S pins properly. Example for INMP441:
```cpp
#define I2S_WS  15
#define I2S_SD  32
#define I2S_SCK 14
```

---

##  Logic Overview

```cpp
1. Read mic data via I2S → convert to dB
2. Determine emotion based on dB thresholds
3. Update OLED expression
4. Set LED strip color (white → green → red scale)
```

---
