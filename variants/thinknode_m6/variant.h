/*
  ThinkNode M6 variant for microReticulum_Firmware
  Based on Meshtastic firmware ELECROW-ThinkNode-M6 variant.h
  (https://github.com/meshtastic/firmware/blob/develop/variants/nrf52840/ELECROW-ThinkNode-M6/variant.h)
  Adapted for use with attermann/microReticulum_Firmware (RNode firmware + embedded RNS).

  MCU:  Nordic nRF52840, Arm Cortex-M4 @ 64 MHz
  LoRa: Semtech SX1262
  GPS:  Quectel L76K (UART, 9600 baud)
  Flash: MX25R1635F, 16MB QSPI
  RTC:  PCF8563 (I2C 0x51)
  Power: Solar MPPT + 2x 18650 (7000 mAh), IP65 enclosure
*/

#ifndef _VARIANT_THINKNODE_M6_
#define _VARIANT_THINKNODE_M6_

#define VARIANT_MCK (64000000ul)
#define USE_LFXO

#include "WVariant.h"

#ifdef __cplusplus
extern "C" {
#endif

// Pin count
#define PINS_COUNT           (48)
#define NUM_DIGITAL_PINS     (48)
#define NUM_ANALOG_INPUTS    (1)
#define NUM_ANALOG_OUTPUTS   (0)

// Helper macro: P0 = 0-31, P1 = 32-63
#define _PINNUM(port, pin)   ((port) * 32 + (pin))

// --- LEDs ---
// LED_POWER: red indicator (P0.12)
// LED_PAIRING / LED_NOTIFICATION: blue indicator (P0.07)
#define LED_POWER      (12)    // P0.12 — power/charging LED (active HIGH)
#define LED_PAIRING    (7)     // P0.07 — data/notify LED (active HIGH)
#define LED_STATE_ON   HIGH
#define LED_STATE_OFF  LOW
// No LED_BLUE equivalent mapped separately; use LED_PAIRING for RX/TX indication

// Arduino/BSP standard LED aliases (required by the nRF52 core & InternalFS)
#define PIN_LED1       LED_POWER
#define PIN_LED2       LED_PAIRING
#define LED_BUILTIN    PIN_LED1
#define LED_CONN       PIN_LED2
#define LED_RED        LED_POWER
#define LED_BLUE       LED_PAIRING

// --- USB power detect ---
#define EXT_PWR_DETECT (13)    // P0.13

// --- Button ---
#define PIN_BUTTON1    (17)    // P0.17 — multifunction (PWR) button

// --- Battery ADC ---
#define PIN_A0         (28)    // P0.28
#define BATTERY_PIN    PIN_A0
#define ADC_CTRL       (11)    // P0.11 — enable ADC measurement (write HIGH)
#define ADC_CTRL_ENABLED 1
static const uint8_t A0 = PIN_A0;
#define ADC_RESOLUTION   14
#define BATTERY_SENSE_SAMPLES 30
#define BATTERY_SENSE_RESOLUTION_BITS 12
#define BATTERY_SENSE_RESOLUTION 4096.0
#undef  AREF_VOLTAGE
#define AREF_VOLTAGE     2.4
#define VBAT_AR_INTERNAL AR_INTERNAL_2_4
#define ADC_MULTIPLIER   (1.75F)
// OCV table (mV) for 18650 2S pack approximation at 10% intervals
#define OCV_ARRAY 4080, 3990, 3935, 3880, 3825, 3770, 3715, 3660, 3605, 3550, 3490

// --- NFC pins (unused, avoid as GPIO) ---
#define PIN_NFC1  (9)
#define PIN_NFC2  (10)

// --- I2C ---
// I2C_SDA: P1.09  (= 32+9 = 41)
// I2C_SCL: P0.08
#define WIRE_INTERFACES_COUNT 1
#define PIN_WIRE_SDA     (32 + 9)   // P1.09 = 41
#define PIN_WIRE_SCL     (8)        // P0.08

// --- Peripheral power enable ---
#define PIN_POWER_EN     (27)       // P0.27

// --- Solar charger status ---
#define EXT_CHRG_DETECT        (15)   // P0.15
#define EXT_CHRG_DETECT_VALUE  LOW    // active LOW = charging

// --- QSPI Flash: MX25R1635F (16MB) ---
#define PIN_QSPI_SCK   (32 + 3)    // P1.03 = 35
#define PIN_QSPI_CS    (23)         // P0.23
#define PIN_QSPI_IO0   (32 + 1)    // P1.01 = 33
#define PIN_QSPI_IO1   (32 + 2)    // P1.02 = 34
#define PIN_QSPI_IO2   (32 + 4)    // P1.04 = 36
#define PIN_QSPI_IO3   (32 + 5)    // P1.05 = 37
#define EXTERNAL_FLASH_DEVICES MX25R1635F
#define EXTERNAL_FLASH_USE_QSPI
#define VDD_FLASH_EN   (21)         // P0.21 — QSPI flash power enable

// --- SX1262 LoRa ---
// SPI pins: P1.15/P1.14/P1.13
// CS:    P1.12 = 44
// DIO1:  P1.06 = 38
// BUSY:  P1.11 = 43
// RESET: P1.10 = 42
// DIO2 used as RF switch, DIO3 drives 3.3V TCXO
#define USE_SX1262
#define SX126X_CS              (32 + 12)  // P1.12 = 44
#define SX126X_DIO1            (32 + 6)   // P1.06 = 38
#define SX126X_BUSY            (32 + 11)  // P1.11 = 43
#define SX126X_RESET           (32 + 10)  // P1.10 = 42
#define SX126X_DIO2_AS_RF_SWITCH
#define SX126X_DIO3_TCXO_VOLTAGE 3.3

// --- GPS: Quectel L76K ---
#define GPS_L76K
#define GPS_BAUDRATE      9600
#define PIN_GPS_EN        (6)    // P0.06 — GPS power enable
#define PIN_GPS_REINIT    (29)   // P0.29
#define PIN_GPS_STANDBY   (30)   // P0.30
#define PIN_GPS_PPS       (31)   // P0.31

// UART1 (GPS)
#define GPS_TX_PIN        (2)    // P0.02 — nRF TX → GPS RX
#define GPS_RX_PIN        (3)    // P0.03 — GPS TX → nRF RX
#define PIN_SERIAL1_TX    GPS_TX_PIN
#define PIN_SERIAL1_RX    GPS_RX_PIN

// --- Secondary UART (SP11 connector) ---
#define PIN_SERIAL2_RX    (22)   // P0.22
#define PIN_SERIAL2_TX    (24)   // P0.24

// --- SPI bus ---
#define SPI_INTERFACES_COUNT 1
#define PIN_SPI_MISO  (32 + 15)  // P1.15 = 47
#define PIN_SPI_MOSI  (32 + 14)  // P1.14 = 46
#define PIN_SPI_SCK   (32 + 13)  // P1.13 = 45

static const uint8_t SS   = (32 + 12);  // SX1262_CS P1.12 = 44
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

// --- RTC ---
// PCF8563 at I2C address 0x51 — shares I2C bus
#define PCF8563_RTC 0x51

// --- Solar ---
#define HAS_SOLAR true

#ifdef __cplusplus
}
#endif
#endif // _VARIANT_THINKNODE_M6_
