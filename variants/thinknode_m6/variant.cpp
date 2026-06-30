/*
  ThinkNode M6 variant.cpp for microReticulum_Firmware
  Adapted from Meshtastic ELECROW-ThinkNode-M6 variant.cpp
*/

#include "variant.h"
#include "wiring_constants.h"
#include "wiring_digital.h"

// nRF52840 physical pin map (P0.xx = index, P1.xx = index+32)
// P0.00/P0.01 are reserved for 32 MHz XTAL; mapped as 0xff
const uint32_t g_ADigitalPinMap[] = {
    // P0 — pins 0 and 1 are hardwired for XTAL, never enable
    0xff, 0xff, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    // P1
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47
};

void initVariant() {
    // Init status LED
    pinMode(LED_PAIRING, OUTPUT);
    digitalWrite(LED_PAIRING, LED_STATE_OFF);

    // Enable QSPI flash power
    pinMode(VDD_FLASH_EN, OUTPUT);
    digitalWrite(VDD_FLASH_EN, HIGH);

    // Disable GPS on boot (GPS enable is active HIGH; pull low until needed)
    // Leave for firmware init to manage.
}
