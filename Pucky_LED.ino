#include <Adafruit_TinyUSB.h>

#define pin 25

void zero() {
  // One CPU Cycle Duration: 15.625 nanoseconds
  noInterrupts();
  // High 400 ns
  NRF_P0->OUT ^= (1UL << 25);
  asm volatile(
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 125 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 250 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 375 ns
    "MOV R0, R0; MOV R0, R0;");                                                                         // 406.25 ns
  NRF_P0->OUT ^= (1UL << 25);
  // Low 850 ns
  asm volatile(
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 125 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 250 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 375 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 500 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 625 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 750 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; ");                        // 843.75 ns
  interrupts();
}

void one() {
  // One CPU Cycle Duration: 15.625 nanoseconds
  noInterrupts();
  // High 800 ns
  NRF_P0->OUT ^= (1UL << 25);
  asm volatile(
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 125 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 250 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 375 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 500 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 625 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 750 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; ");                                                // 796.875 ns
  NRF_P0->OUT ^= (1UL << 25);
  // Low 450 ns
  asm volatile(
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 125 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 250 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; "  // 375 ns
    "MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; MOV R0, R0; ");                                    // 453.125 ns
  interrupts();
}

void showColor(uint8_t r, uint8_t g, uint8_t b) {
  for (uint8_t ix = 0; ix < 8; ix++) {
    if (g & 0b10000000) one();
    else zero();
    g = g << 1;
  }
  for (uint8_t ix = 0; ix < 8; ix++) {
    if (r & 0b10000000) one();
    else zero();
    r = r << 1;
  }
  for (uint8_t ix = 0; ix < 8; ix++) {
    if (b & 0b10000000) one();
    else zero();
    b = b << 1;
  }
}

void red() {
  // RGB: 0xFF 0x00 0x00
  // GRB: 0x00 0xFF 0x00
  for (uint8_t ix = 0; ix < 16; ix++)
    one();
  for (uint8_t ix = 0; ix < 8; ix++)
    zero();
}

void green() {
  // RGB: 0x00 0xFF 0x00
  // GRB: 0xFF 0x00 0x00
  for (uint8_t ix = 0; ix < 8; ix++)
    one();
  for (uint8_t ix = 0; ix < 16; ix++)
    zero();
}

void blue() {
  // RGB: 0x00 0x00 0xFF
  // GRB: 0x00 0x00 0xFF
  for (uint8_t ix = 0; ix < 16; ix++)
    zero();
  for (uint8_t ix = 0; ix < 8; ix++)
    one();
}

void yellow() {
  // RGB: 0xFF 0xFF 0x00
  // GRB: 0xFF 0xFF 0x00
  for (uint8_t ix = 0; ix < 16; ix++)
    one();
  for (uint8_t ix = 0; ix < 8; ix++)
    zero();
}

void white() {
  for (uint8_t ix = 0; ix < 23; ix++)
    one();
}

void black() {
  for (uint8_t ix = 0; ix < 23; ix++)
    zero();
}

void setup() {
  Serial.begin(115200);
  pinMode(25, OUTPUT);
  digitalWrite(pin, LOW);
  yellow();
  delay(1000);
  blue();
  delay(1000);
  white();
  delay(1000);
  Serial.println("Ready...");
}

void loop() {
  green();
  delay(1000);
  black();
  delay(1000);
  yellow();
  delay(1000);
  black();
  delay(1000);
}
