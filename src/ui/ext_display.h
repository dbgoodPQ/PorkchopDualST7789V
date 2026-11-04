#ifndef PORK_EXT_DISPLAY_H
#define PORK_EXT_DISPLAY_H

// External 2.0"/2.4" ST7789V (240x320) display for M5PORKCHOP on Cardputer-ADV.
// Dual-screen mode: action modes render full-screen (fit) on the TFT; the
// built-in screen always shows the current view natively (incl. the menu).
//
// Config: rotation via EXT_ROTATION (landscape 320x240), invert=true (ST7789 default),
// rgb_order=false. Shares the SD card's SPI bus -> FSPI/SPI2_HOST, bus_shared=true.
//
// Wiring (EXT header): CS=GPIO5 RST=GPIO3 DC=GPIO6 MOSI=GPIO14 SCK=GPIO40
//
// ====================================================================
// PORTED FROM ILI9341 -> ST7789V
// --------------------------------------------------------------------
// The whole UI is built for a 320x240 LANDSCAPE panel, so this expects a
// 240x320 ST7789V module (the common 2.0" / 2.4" boards). LovyanGFX ships a
// built-in Panel_ST7789 with its own init sequence, so no hand-rolled command
// table is needed (unlike the ILI9341, which M5GFX had no panel class for).
//
// If your image looks like a photo-negative: toggle `invert`.
// If red/blue are swapped:                    toggle `rgb_order`.
// If you have a 240x240 or 240x135 ST7789V:   set panel_width/height and the
//   offset_x/offset_y for your module -- and note the landscape layout/scale
//   constants assume 240x320, so a smaller panel needs layout rework too.
// ====================================================================

#include <M5Unified.h>
#include <lgfx/v1/panel/Panel_ST7789.hpp>

// ---- Display rotation (easy knob) ----------------------------------
// LovyanGFX rotations: 0-3 = 0/90/180/270; 4-7 = the same but H-mirrored.
// Landscape 320x240 values are 1, 3 (no mirror) and 5, 7 (mirrored).
// The ILI9341 build used 7. On ST7789V try these if the image is wrong:
//   1 = landscape, no mirror            (default here)
//   3 = landscape, no mirror, 180° flip (use if rot 1 is upside-down)
//   5 / 7 = mirrored versions of 1 / 3  (right-to-left text)
// Just change this one number and re-flash.
static constexpr uint8_t EXT_ROTATION = 1;

// Native-clone placement (used only as a heap fallback if scaling can't alloc).
static constexpr int EXT_CLONE_OX = 40;   // (320 - 240) / 2
static constexpr int EXT_CLONE_OY = 52;   // (240 - 135) / 2

// ---- External ST7789V panel (uses LovyanGFX's built-in Panel_ST7789) ----
class LGFX_ExtST7789V : public lgfx::v1::LGFX_Device {
    lgfx::v1::Panel_ST7789 _panel_instance;
    lgfx::v1::Bus_SPI      _bus_instance;
public:
    LGFX_ExtST7789V() {
        {   auto cfg = _bus_instance.config();
            cfg.spi_host    = SPI2_HOST;     // FSPI — SAME host as the SD card
            cfg.spi_mode    = 0;
            cfg.freq_write  = 40000000;      // ST7789 tolerates 40MHz; watch SD stability
            cfg.freq_read   = 16000000;
            cfg.spi_3wire   = true;
            cfg.use_lock    = true;
            cfg.dma_channel = 0;
            cfg.pin_sclk    = 40;
            cfg.pin_mosi    = 14;
            cfg.pin_miso    = -1;
            cfg.pin_dc      = 6;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        {   auto cfg = _panel_instance.config();
            cfg.pin_cs           = 5;
            cfg.pin_rst          = 3;
            cfg.pin_busy         = -1;
            cfg.memory_width     = 240;
            cfg.memory_height    = 320;
            cfg.panel_width      = 240;
            cfg.panel_height     = 320;
            cfg.offset_x         = 0;
            cfg.offset_y         = 0;
            cfg.offset_rotation  = 0;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits  = 1;
            cfg.readable         = false;
            cfg.invert           = true;     // ST7789 defaults to inverted (ILI9341 was false)
            cfg.rgb_order        = false;    // flip to true if red/blue are swapped
            cfg.dlen_16bit       = false;
            cfg.bus_shared       = true;     // shared with SD card
            _panel_instance.config(cfg);
        }
        setPanel(&_panel_instance);
    }
};

// Back-compat alias so existing references (ext_display.cpp) keep compiling.
using LGFX_ExtILI9341 = LGFX_ExtST7789V;

extern LGFX_ExtST7789V extDisplay;

// Band effect selector (cycled by left/right keys in action modes)
extern uint8_t extBandEffect;
void extCycleBandEffect(int dir);

void extInit();   // call once inside Display::init()

// Push the three Porkchop canvases to the external TFT.
//   ScaledFit : composite + scale-to-fit (320x180, letterboxed). For action modes.
//   Native    : centered 1:1 (heap fallback only).
void extPushScaledFit(M5Canvas& top, M5Canvas& main, M5Canvas& bottom);
void extPushNative(M5Canvas& top, M5Canvas& main, M5Canvas& bottom);

// Low-level access for a custom-composited frame (e.g. the menu backdrop):
void extMenuPig();            // big animated pig shown on the TFT during menus/text screens
void extBootIntro();          // one-time matrix->pig reveal animation, call once at boot

// SQUACH-CAM trail-cam options (independent). The band effect is ALWAYS available in the
// , / cycle regardless of these.
extern volatile bool g_audioMicActive;   // true while the menu mic holds the I2S bus
static constexpr bool SQUACHCAM_BOOT = true;    // true: squach trail-cam power-on reveal at boot
static constexpr bool SQUACHCAM_MENU = false;   // false: keep the matrix/pig menu backdrop
void extMenuSquachCam();      // SQUACH-CAM full-screen menu backdrop
void extBootIntroSquach();    // SQUACH-CAM trail-cam power-on reveal

#endif // PORK_EXT_DISPLAY_H
