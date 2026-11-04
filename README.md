# PorkchopDualST7789V

A fork of [skizzophrenic/M5PORKCHOP_DualScreen](https://github.com/skizzophrenic/M5PORKCHOP_DualScreen)
that switches the external screen from an ILI9341 to a **Waveshare 2inch LCD module (ST7789V, 240x320)**.

Target device: **M5Stack Cardputer**. The dual-screen UI is unchanged from the upstream fork; only the
external panel driver and its settings differ.

## What changed

- The external panel uses LovyanGFX's built-in `Panel_ST7789` instead of the hand-written ILI9341 init table.
- `invert = true` (the ST7789V needs inverted colours) and `freq_write = 40 MHz`.
- Rotation 7 is kept (the scaled-push geometry is tuned for it). The ST7789V's left/right mirror is undone in
  software with `EXT_FLIP_X`.

All settings are in `src/ui/ext_display.h`.

## Dual-screen layout

- **External screen (320x240 landscape):** action modes (IDLE, OINK, DNH, WARHOG, PIGGYBLUES, SPECTRUM, BACON,
  FILE_TRANSFER, PIGSYNC, CHARGING) run full-screen, scaled to fit, with thin effect bands top and bottom.
  In the menu it shows a full-screen Matrix rain with an animated pig.
- **Built-in screen:** always shows the current view, including the menu and all text screens
  (Settings, Captures, Stats, and so on).
- Cycle the band effects with `,` and `/` in action modes.

## Wiring (Cardputer EXT header → Waveshare 2inch)

| Waveshare pin | Cardputer GPIO | Notes |
| --- | --- | --- |
| DIN (MOSI) | G14 | shared with the SD card |
| CLK (SCK) | G40 | shared with the SD card |
| CS | G5 | |
| DC | G6 | |
| RST | G3 | |
| BL | 3V3 | always on |
| VCC | 3V3 | set the module's voltage selector to 3.3V |
| GND | GND | |

MISO is not connected.

**Before flashing:** turn GPS off in Settings. With no GPS module, it would drive GPIO 3/5/6 (the panel's RST/DC/CS).

## Build and flash

With VS Code and the PlatformIO extension:

```
git clone https://github.com/dbgoodPQ/PorkchopDualST7789V
cd PorkchopDualST7789V
pio run -e m5cardputer -t upload
```

The **build** GitHub Action also produces `firmware.bin` as a downloadable artifact.

## If something looks wrong

| Symptom | Try |
| --- | --- |
| Colours look like a photo negative | Toggle `invert` in `src/ui/ext_display.h` |
| Red and blue swapped | Toggle `rgb_order` |
| Picture mirrored left/right | Toggle `EXT_FLIP_X` |
| SD card errors or garbled picture | Lower `freq_write` (for example to 27 MHz) |

Implementation notes, effects and tuning options are in [EXT_DISPLAY_NOTES.md](EXT_DISPLAY_NOTES.md).

## Disclaimer

This firmware is for education and authorised security testing only. You are responsible for following
the radio and privacy laws where you use it.

## Credits

M5PORKCHOP by 0ct0 and the dual-screen port by
[skizzophrenic](https://github.com/skizzophrenic/M5PORKCHOP_DualScreen). MIT License — see [LICENSE](LICENSE).
