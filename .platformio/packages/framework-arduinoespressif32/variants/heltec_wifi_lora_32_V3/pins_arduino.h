#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

#define DISPLAY_HEIGHT 64
#define DISPLAY_WIDTH 128

#define USB_VID 0x303a
#define USB_PID 0x1001

static const uint8_t LED_BUILTIN = 35;
#define LED_BUILTIN LED_BUILTIN

static const uint8_t TX = 43;
static const uint8_t RX = 44;

static const uint8_t SDA = 41;
static const uint8_t SCL = 42;

static const uint8_t SS = 8;
static const uint8_t MOSI = 10;
static const uint8_t MISO = 11;
static const uint8_t SCK = 9;

static const uint8_t A0 = 1;
static const uint8_t A1 = 2;
static const uint8_t A2 = 3;
static const uint8_t A3 = 4;
static const uint8_t A4 = 5;
static const uint8_t A5 = 6;
static const uint8_t A6 = 7;
static const uint8_t A7 = 8;
static const uint8_t A8 = 9;
static const uint8_t A9 = 10;
static const uint8_t A10 = 11;
static const uint8_t A11 = 12;
static const uint8_t A12 = 13;
static const uint8_t A13 = 14;
static const uint8_t A14 = 15;
static const uint8_t A15 = 16;
static const uint8_t A16 = 17;
static const uint8_t A17 = 18;
static const uint8_t A18 = 19;
static const uint8_t A19 = 20;

static const uint8_t T1 = 1;
static const uint8_t T2 = 2;
static const uint8_t T3 = 3;
static const uint8_t T4 = 4;
static const uint8_t T5 = 5;
static const uint8_t T6 = 6;
static const uint8_t T7 = 7;
static const uint8_t T8 = 8;
static const uint8_t T9 = 9;
static const uint8_t T10 = 10;
static const uint8_t T11 = 11;
static const uint8_t T12 = 12;
static const uint8_t T13 = 13;
static const uint8_t T14 = 14;

static const uint8_t Vext = 36;
static const uint8_t LED = 35; // LED_BUILTIN

static const uint8_t CS_LoRa = 8;    // LoRa_NSS
static const uint8_t SCK_LoRa = 9;   // LoRa_SCK
static const uint8_t MOSI_LoRa = 10; // LoRa_MOSI
static const uint8_t MISO_LoRa = 11; // LoRa_MISO
static const uint8_t RST_LoRa = 12;  // LoRa_RST
static const uint8_t BUSY_LoRa = 13; // LoRa_Busy
static const uint8_t DIO1_LoRa = 14; // LoRa_DIO1

static const uint8_t SDA_OLED = 17; // OLED_SDA
static const uint8_t SCL_OLED = 18; // OLED_SCL
static const uint8_t RST_OLED = 21; // OLED_RST

#endif /* Pins_Arduino_h */
