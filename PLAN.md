# ThinkNode M6 — microReticulum_Firmware Port Plan

Goal: add first-class ThinkNode M6 support to [attermann/microReticulum_Firmware](https://github.com/attermann/microReticulum_Firmware) so the board runs as a fully self-contained Reticulum transport node (embedded RNS + LoRa radio) with solar, GPS, and BLE.

Working branch: `feature/thinknode-m6`
Forks: `emuehlstein/microReticulum_Firmware` (firmware), `emuehlstein/microReticulum` (library)

---

## Hardware Reference

| Subsystem | Part | Notes |
|-----------|------|-------|
| MCU | Nordic nRF52840, Cortex-M4 @ 64 MHz | 256 KB SRAM, 1 MB flash |
| LoRa | Semtech SX1262 | SPI, DIO2=RF switch, DIO3=TCXO 3.3V |
| GPS | Quectel L76K | UART1 9600 baud; PIN_GPS_EN=P0.06 |
| Flash | MX25R1635F 16 MB | QSPI; VDD_FLASH_EN=P0.21 |
| RTC | PCF8563 | I2C 0x51 |
| Battery ADC | Internal | P0.28; ADC_CTRL=P0.11 (active HIGH) |
| Solar | MPPT; EXT_CHRG_DETECT=P0.15 (active LOW) | 6W panel, 7000 mAh |
| LEDs | LED_POWER=P0.12 (red), LED_PAIRING=P0.07 (blue) | Active HIGH |
| Button | P0.17 | Multifunction; long-press on/off |
| DFU | Double-tap RESET within 250 ms, plug USB | |
| I2C | SDA=P1.09, SCL=P0.08 | |
| SP11 UART | RX=P0.22, TX=P0.24 | External sensor connector |
| SP11 I2C | shares main I2C bus | |

### Full Pin Map

```
SX1262_CS      P1.12 = 44
SX1262_DIO1    P1.06 = 38  (IRQ)
SX1262_BUSY    P1.11 = 43
SX1262_RESET   P1.10 = 42
SX1262_SCK     P1.13 = 45
SX1262_MOSI    P1.14 = 46
SX1262_MISO    P1.15 = 47

GPS_TX (→ GPS RX)  P0.02
GPS_RX (← GPS TX)  P0.03
GPS_EN             P0.06  (active HIGH)
GPS_REINIT         P0.29
GPS_STANDBY        P0.30
GPS_PPS            P0.31

I2C_SDA            P1.09 = 41
I2C_SCL            P0.08

UART2_RX (SP11)    P0.22
UART2_TX (SP11)    P0.24

LED_POWER          P0.12  (red,  active HIGH)
LED_PAIRING        P0.07  (blue, active HIGH)
BUTTON_USR         P0.17

BATTERY_ADC        P0.28
ADC_CTRL           P0.11  (HIGH = enable measurement)
EXT_PWR_DETECT     P0.13  (USB present)
EXT_CHRG_DETECT    P0.15  (active LOW = solar/USB charging)
PIN_POWER_EN       P0.27  (peripheral power rail)

QSPI_SCK           P1.03 = 35
QSPI_CS            P0.23
QSPI_IO0           P1.01 = 33
QSPI_IO1           P1.02 = 34
QSPI_IO2           P1.04 = 36
QSPI_IO3           P1.05 = 37
VDD_FLASH_EN       P0.21

NFC1               P0.09  (avoid as GPIO)
NFC2               P0.10  (avoid as GPIO)
```

---

## What's Done (in this branch)

- [x] `variants/thinknode_m6/variant.h` — full pin definitions, matches Meshtastic source
- [x] `variants/thinknode_m6/variant.cpp` — pin map + initVariant()
- [x] `variants/thinknode_m6/WVariant.h` — copied from rak4630 (Arduino nRF52 BSP header)
- [x] `boards/thinknode_m6.json` — PlatformIO board definition (nRF52840, adafruit BSP, nrfutil upload)
- [x] `Boards.h` — added `PRODUCT_THINKNODE_M6 0x20`, `BOARD_THINKNODE_M6 0x52`, model defines, and full nRF52 config block (SX1262 pins, HAS_* flags, BLE identity)
- [x] `platformio.ini` — `[env:thinknode_m6]` environment extending `embedded-nrf52-minimal`

---

## TODO — Ordered by Priority

### P0: Build validation
- [ ] Install PlatformIO, run `pio run -e thinknode_m6` — expect clean compile
- [ ] Fix any symbol conflicts or missing #ifdef guards that surface from compile
- [ ] Check `Boards.h` nRF52 `#else` error guard doesn't catch M6 (add `|| BOARD_MODEL == BOARD_THINKNODE_M6` to any existing nRF52 `#if` chains)

### P1: Flash and basic RNode behavior
- [ ] Flash via DFU (`pio run -e thinknode_m6 -t upload`) — double-tap RESET first
- [ ] Connect `rnodeconf` and verify device identifies as a valid RNode
- [ ] Confirm LoRa RX/TX LED blink on air traffic (use `--check` or `--frequency 915000000 --bw 125000 --sf 8 --cr 5 --txp 17`)
- [ ] Verify BLE advertising shows "ThinkNode M6"

### P2: Transport node (embedded RNS)
- [ ] Switch to TNC mode: `rnodeconf --tnc --freq 910525000 --bw 62500 --sf 7 --cr 5 --txp 17 /dev/ttyACMx`
- [ ] Observe serial log — should show "TRANSPORT" in AirTime panel log
- [ ] Confirm packet routing with another RNS node on Chicago freq (910.525 MHz / BW 62.5 / SF7 / CR5)

### P3: GPS integration
- [ ] GPS is not part of the base RNode firmware; this is an extension point
- [ ] Options:
  a. **NomadNet/LXMF with GPS** — if attermann adds GPS position beacon support, wire PIN_GPS_EN + UART1
  b. **Standalone** — GPS feeds position to a separate process (e.g., Python script via USB serial)
  c. **Custom** — add GPS NMEA parsing to firmware; emit position as RNS announce metadata
- [ ] For now: ensure GPS doesn't interfere — call `digitalWrite(PIN_GPS_EN, LOW)` on boot (handled in variant.cpp)

### P4: Solar / power management
- [ ] Map `EXT_CHRG_DETECT` (P0.15) to a readable status in firmware
- [ ] Map `EXT_PWR_DETECT` (P0.13) for USB detect
- [ ] Consider deep sleep: wake on BUTTON (P0.17 sense LOW) or solar charge detect (EXT_CHRG_DETECT sense LOW when battery low)
- [ ] `variant_shutdown()` pattern from Meshtastic (see reference below) — adapt for RNode sleep path

### P5: External flash persistence
- [ ] Verify `DUSTORE_USE_FLASHFS` works with MX25R1635F via QSPI
- [ ] Test path table persistence across reboots
- [ ] `VDD_FLASH_EN` must be HIGH before flash access (initVariant handles this)

### P6: SP11 sensor UART
- [ ] UART2 (P0.22 RX / P0.24 TX) available for external sensors via SP11 waterproof connector
- [ ] Future: pipe sensor telemetry as RNS messages

---

## Reference: Meshtastic variant_shutdown() pattern

```cpp
void variant_shutdown() {
    for (int pin = 0; pin < 48; pin++) {
        if (pin == PIN_GPS_EN || pin == ADC_CTRL || pin == PIN_BUTTON1 ||
            pin == PIN_SPI_MISO || pin == PIN_SPI_MOSI || pin == PIN_SPI_SCK ||
            pin == SX126X_CS || pin == SX126X_RESET || pin == SX126X_BUSY || pin == SX126X_DIO1)
            continue;
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }
    digitalWrite(PIN_GPS_EN, LOW);
    digitalWrite(ADC_CTRL, LOW);

    // Wake on button press
    nrf_gpio_cfg_input(PIN_BUTTON1, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_sense_set(PIN_BUTTON1, NRF_GPIO_PIN_SENSE_LOW);

    // Wake on solar charge (low battery case only)
    if (isLowBattery()) {
        nrf_gpio_cfg_input(EXT_CHRG_DETECT, NRF_GPIO_PIN_PULLUP);
        nrf_gpio_cfg_sense_set(EXT_CHRG_DETECT, NRF_GPIO_PIN_SENSE_LOW);
    }
}
```
Implement in `variants/thinknode_m6/variant.cpp` when sleep support is added.

---

## Key Links

- attermann/microReticulum_Firmware: <https://github.com/attermann/microReticulum_Firmware>
- attermann/microReticulum (library): <https://github.com/attermann/microReticulum>
- Meshtastic ThinkNode M6 variant (pinout source): <https://github.com/meshtastic/firmware/tree/develop/variants/nrf52840/ELECROW-ThinkNode-M6>
- Elecrow wiki: <https://www.elecrow.com/pub/wiki/ThinkNode_M6_Outdoor_Solar_Power_for_Meshtastic_Powered_By_nRF52840_Supports_GPS.html>
- Elecrow user manual: <https://www.elecrow.com/download/product/LMM14106D/ThinkNode_M6_Outdoor_Solar_Power_for_Meshtastic_User_Manual.pdf>
- RAK4631 variant (reference nRF52 board): `variants/rak4630/`
- LilyGO T-Echo variant (reference nRF52 with sleep): `variants/lilygo_t_echo/`

---

## Notes

**Why attermann/microReticulum_Firmware over liberatedsystems/RNode_Firmware_CE?**
attermann's fork adds the embedded microReticulum stack — the device becomes a self-contained RNS transport node, not just a USB radio modem. That's the interesting case for a solar-powered outdoor node.

**RNode model ID 0x52** — not registered upstream. Fine for a fork; if this gets PR'd to attermann, coordinate model ID assignment.

**Chicago Offline note**: at 910.525 MHz / BW 62.5 / SF7 / CR5 this board would be a solid fixed observer. With solar + IP65 it could run indefinitely on a rooftop or pole as both an RNS transport and a MeshCore observer (run two firmware variants depending on use case).
