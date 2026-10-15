# Ferris Sweep (Half Swept) – QMK keymap

My personal [QMK](https://qmk.fm) keymap for a **Ferris Sweep "Half Swept"**:
a 34-key, column-staggered split keyboard (3×5 grid + 2 thumb keys per half),
used **wired** (USB to one half + TRRS cable between the halves). The main machine is
**Windows** (US QWERTY), with occasional **macOS** use through an automatic Mac/Win mode.
I mostly use it for programming.

This repo holds only the *keymap folder* (`keymap.c`, `config.h`, `rules.mk`).
The keyboard definition (pins, matrix, split setup) lives upstream in
[`qmk_firmware/keyboards/ferris/sweep`](https://github.com/qmk/qmk_firmware/tree/master/keyboards/ferris/sweep).
To build anything you combine the two (see [Building](#3-building-the-firmware)).

---

## Table of contents

1. [Hardware overview](#1-hardware-overview)
2. [Repo layout & how the files fit together](#2-repo-layout)
3. [Building the firmware](#3-building-the-firmware)
4. [Flashing both halves (wired)](#4-flashing-both-halves-wired)
5. [The layout](#5-the-layout)
6. [Mac/Win mode](#6-macwin-mode)
7. [Behaviour & tuning](#7-behaviour--tuning)
8. [How to make common changes](#8-how-to-make-common-changes)
9. [Further suggestions (not applied)](#9-further-suggestions-not-applied)
10. [Troubleshooting](#10-troubleshooting)
11. [References](#11-references)

---

## 1. Hardware overview

| Thing | Value |
|---|---|
| PCB | Sweep "Half Swept": same as the Sweep 2 but with a **reversible Pro Micro footprint** ([davidphilipbarr/Sweep](https://github.com/davidphilipbarr/Sweep)) |
| Keys | 34 (per half: 3 rows × 5 columns + 2 thumbs) |
| Controllers | 2 × **ATmega32U4 Pro Micro** (Caterina bootloader, flashed with `avrdude`, `.hex` files) |
| Flash space | 28 672 bytes usable. This keymap uses ~19.6 KB (68%) with LTO |
| QMK keyboard name | `ferris/sweep` (covers all Sweep variants) |
| QMK layout macro | `LAYOUT_split_3x5_2` |
| Matrix | *Direct pins* (every switch has its own MCU pin, so no diodes) |
| Split comms | Serial over the TRRS cable, pin `D2` |
| Handedness | Not configured, so QMK defaults to `MASTER_LEFT`: **the half with the USB cable is the left half** |
| Lighting | None (the Sweep has no LEDs) |
| Host keyboard layout | **US (QWERTY)** on the OS. All symbol keycodes assume this |

---

## 2. Repo layout

```
.
├── keymap.c    # layers, home-row-mod aliases, combos, Backspace repeat, Mac/Win detection
├── config.h    # compile-time tuning: tapping term, mouse speed, split USB detection
├── rules.mk    # feature switches: combos, OS detection, LTO
└── README.md
```

How QMK combines everything at build time:

```
qmk_firmware/keyboards/ferris/sweep/keyboard.json   ← hardware (pins, split, layout macro)
qmk_firmware/keyboards/ferris/sweep/keymaps/<km>/   ← THIS REPO (symlinked in)
        └─ your config.h / rules.mk are applied on top of the keyboard's defaults
```

Features already enabled by the keyboard's `keyboard.json` (you don't need them in `rules.mk`):
`bootmagic`, `extrakey` (media/volume/brightness), `mousekey`, `unicode`.
`MAGIC_ENABLE` (needed for the Ctrl↔Cmd swap) is on by default in QMK.

---

## 3. Building the firmware

This Mac uses **Nix** for the toolchain, so nothing is installed globally. The nixpkgs `qmk`
package bundles the QMK CLI with `avr-gcc`, `avrdude` and friends.

### 3.1 One-time setup

```bash
git clone --depth 1 https://github.com/qmk/qmk_firmware.git ~/qmk_firmware
```

```bash
git -C ~/qmk_firmware submodule update --init --depth 1 lib/lufa lib/printf
```

(For this AVR board, only the `lufa` and `printf` submodules are needed. Use
`git submodule update --init --recursive` if you ever build for ARM/RP2040.)

Link this repo in as a keymap called `rattatui`. Upstream Sweep has no `keymaps/` folder,
so create it first:

```bash
mkdir -p ~/qmk_firmware/keyboards/ferris/sweep/keymaps
```

```bash
ln -s ~/projects/ferris ~/qmk_firmware/keyboards/ferris/sweep/keymaps/rattatui
```

Tell the QMK CLI where everything is, so you can drop `-kb` / `-km` later:

```bash
nix shell nixpkgs#qmk --command qmk config user.qmk_home=$HOME/qmk_firmware user.keyboard=ferris/sweep user.keymap=rattatui
```

### 3.2 Compile

```bash
nix shell nixpkgs#qmk --command qmk compile -kb ferris/sweep -km rattatui
```

Output: `~/qmk_firmware/ferris_sweep_rattatui.hex`. The build ends with a size line, e.g.
`The firmware size is fine - 19634/28672 (68%, 9038 bytes free)`.

Tips:
- Run `nix shell nixpkgs#qmk` once to get a shell with `qmk` on `PATH`, then use plain
  `qmk compile` / `qmk flash` in it.
- `qmk clean` if a build behaves oddly after updating QMK.
- `qmk lint -kb ferris/sweep -km rattatui` only complains that `keymap.c` has no license
  header. That matters only for upstream PRs, so ignore it.
- Without Nix: `brew install qmk/qmk/qmk && qmk setup` gives you the same commands.

### 3.3 Keeping QMK up to date

```bash
git -C ~/qmk_firmware pull && git -C ~/qmk_firmware submodule update --init --depth 1 lib/lufa lib/printf
```

QMK puts out a breaking-changes release roughly every quarter (late Feb/May/Aug/Nov).
Read the changelog at <https://docs.qmk.fm/ChangeLog> after a pull. For example, the
2025-08-31 release removed the old `KC_MS_*` / `KC_BTN*` mouse names this keymap used to have.

---

## 4. Flashing both halves (wired)

### Rules of thumb

- **Never plug or unplug the TRRS cable while either half has power.** TRRS jacks short
  pins as they slide in, which can kill a Pro Micro. Unplug USB first.
- **USB always goes into the LEFT half** for daily use. Handedness is `MASTER_LEFT`, so
  whichever half has USB *thinks it is the left half*.
- Both halves run the **same `.hex`**. Only the half on USB (the master) processes the
  keymap; the other half just reads its switches and sends them over TRRS.
  - **Keymap-only change:** flashing the left half is enough.
  - **After a QMK update:** flash **both** halves, since the split protocol has to match.

### Entering the bootloader

Any of these works. The Pro Micro's Caterina bootloader then waits about 8 seconds:

| Method | How |
|---|---|
| **`QK_BOOT` key** (easiest) | Hold **Enter** (MEDIA layer) and press **Q** |
| Bootmagic | Unplug, hold the top-outer key (**Q** on the left half, **P** on the right half) and plug USB in. *Also clears EEPROM, which resets the saved Mac/Win toggle* |
| Reset button | Press the reset button on the PCB once (some clones need a quick double-press) |

`QK_BOOT` only works on the half that has USB. That's fine for the left half. For the
right half, use bootmagic or the reset button.

### Step by step (macOS)

> [!IMPORTANT]
> On macOS, `qmk flash` can fail with
> `cannot open port /dev/tty.usbmodem…: Resource busy`. QMK's script
> (`platforms/avr/flash.mk`) only looks for `/dev/tty.*` devices, while avrdude needs the
> `/dev/cu.*` one. Nothing gets written when this happens, so it's harmless. Use the
> compile + avrdude flow below instead, which is tested on this setup.

1. Unplug USB, then unplug the TRRS cable.
2. Build:
   ```bash
   nix shell nixpkgs#qmk --command qmk compile -kb ferris/sweep -km rattatui
   ```
3. Make sure no other USB-serial device is attached. This should print *no matches found*:
   ```bash
   ls /dev/cu.usbmodem*
   ```
4. Plug USB into the **left** half only and run:
   ```bash
   nix shell nixpkgs#avrdude --command sh -c 'echo "Press reset now"; until ls /dev/cu.usbmodem* >/dev/null 2>&1; do sleep 0.2; done; sleep 1; avrdude -p atmega32u4 -c avr109 -P "$(ls /dev/cu.usbmodem* | head -1)" -U flash:w:$HOME/qmk_firmware/ferris_sweep_rattatui.hex:i'
   ```
5. At *"Press reset now"*, enter the bootloader (see the table). Success looks like
   `Writing | #### … 100%`, `Reading | #### … 100%`, then `19634 bytes of flash verified`.
   If the 8-second window closes first, run the command again.
6. **If the right half needs it** (after a QMK update): unplug, plug USB into the
   **right** half, run the same command, and enter the bootloader with the reset button
   or by holding **P** while plugging in. It's the same `.hex` for both halves.
7. Unplug USB, connect TRRS, and plug USB into the left half.

On Linux/Windows (QMK MSYS), plain `qmk flash -kb ferris/sweep -km rattatui` does steps 2–5 in one go.

### Backup & restore

Before risky changes, read the current firmware off a half (it only reads, nothing is changed):

```bash
nix shell nixpkgs#avrdude --command sh -c 'echo "Press reset now"; until ls /dev/cu.usbmodem* >/dev/null 2>&1; do sleep 0.2; done; sleep 1; avrdude -p atmega32u4 -c avr109 -P "$(ls /dev/cu.usbmodem* | head -1)" -U flash:r:$HOME/ferris-left-backup.hex:i'
```

A good dump is about 78 KB (the full 32 KB chip). To restore it, run the flash command
above with the backup file in place of `ferris_sweep_rattatui.hex`. The dump includes the
bootloader area, which protects itself from being overwritten, so a verify mismatch at
the very end of flash is harmless.

### Flashing without the CLI (e.g. from Windows)

[QMK Toolbox](https://github.com/qmk/qmk_toolbox): open the `.hex`, tick *Auto-Flash*, and
put the half into bootloader mode.

### Optional: EE_HANDS (USB into either half)

1. Add `#define EE_HANDS` to `config.h`.
2. Flash each half **once** with its handedness written to EEPROM. On Linux/Windows:
   ```bash
   qmk flash -kb ferris/sweep -km rattatui -bl avrdude-split-left
   ```
   ```bash
   qmk flash -kb ferris/sweep -km rattatui -bl avrdude-split-right
   ```
   On macOS, use the avrdude command above with an extra
   `-U eeprom:w:$HOME/qmk_firmware/quantum/split_common/eeprom-lefthand.eep:i`
   (or `eeprom-righthand.eep` on the right half).
3. Later flashes can use the plain command. Bootmagic clears EEPROM, so flash the
   split targets again after using it.

---

## 5. The layout

Legend: `▽` = transparent (falls through to the layer below), blank = `KC_NO` (does nothing),
`hold` marks the thumb that is holding the current layer. Home-row mods show as
`tap/hold`. Mod names are for **Windows mode**. In Mac mode every Win↔Ctrl is swapped
(see [§6](#6-macwin-mode)).

### BASE: QWERTY + home-row mods

```
┌──────┬──────┬──────┬──────┬──────┐          ┌──────┬──────┬──────┬──────┬──────┐
│  Q   │  W   │  E   │  R   │  T   │          │  Y   │  U   │  I   │  O   │  P   │
├──────┼──────┼──────┼──────┼──────┤          ├──────┼──────┼──────┼──────┼──────┤
│ A    │ S    │ D    │ F    │  G   │          │  H   │ J    │ K    │ L    │ ;    │
│ /Win │ /Alt │ /Ctl │ /Sft │      │          │      │ /Sft │ /Ctl │ /Alt │ /Win │
├──────┼──────┼──────┼──────┼──────┤          ├──────┼──────┼──────┼──────┼──────┤
│  Z   │  X   │  C   │  V   │  B   │          │  N   │  M   │  ,   │  .   │  /   │
└──────┴──────┴──────┼──────┼──────┤          ├──────┼──────┼──────┴──────┴──────┘
                     │ Tab  │Space │          │ Bksp │Enter │
                     │ /SYM │ /NAV │          │ /NUM │/MEDIA│
                     └──────┴──────┘          └──────┴──────┘
Combos:  Q+W → Esc      O+P → F4   (e.g. hold Alt on S, then O+P = Alt+F4)
```

- Mod order is **GACS** (GUI, Alt, Ctrl, Shift) from pinky to index, mirrored on both hands.
- The right-hand Alt is deliberately **Left** Alt (`LALT_T`). On Windows, Right Alt is
  *AltGr*, which would type special characters instead of acting as Alt.
- **Backspace auto-repeats** with tap-then-hold: tap it, then press and hold it again
  within 200 ms. A plain hold still activates NUM.

### NAV: arrows + mouse (hold Space)

```
┌──────┬──────┬──────┬──────┬──────┐          ┌──────┬──────┬──────┬──────┬──────┐
│      │      │      │      │      │          │Accel2│Whl ← │Whl ↓ │Whl ↑ │Whl → │
├──────┼──────┼──────┼──────┼──────┤          ├──────┼──────┼──────┼──────┼──────┤
│ Win  │ Alt  │ Ctl  │ Sft  │      │          │Accel1│  ←   │  ↓   │  ↑   │  →   │
├──────┼──────┼──────┼──────┼──────┤          ├──────┼──────┼──────┼──────┼──────┤
│      │      │      │      │      │          │Accel0│Mse ← │Mse ↓ │Mse ↑ │Mse → │
└──────┴──────┴──────┼──────┼──────┤          ├──────┼──────┼──────┴──────┴──────┘
                     │  ▽   │ hold │          │Click │RClick│
                     └──────┴──────┘          └──────┴──────┘
```

Arrows sit on **J K L ;** (←↓↑→), not vim's HJKL. `Accel0/1/2` act as speed modifiers
while held (slow / medium / fast). The left-hand mods let you do Ctrl+←, Shift+→ and so on
with one hand per side.

### SYM: symbols (hold Tab)

```
┌──────┬──────┬──────┬──────┬──────┐          ┌──────┬──────┬──────┬──────┬──────┐
│      │      │      │      │      │          │  *   │  (   │  )   │  -   │  =   │
├──────┼──────┼──────┼──────┼──────┤          ├──────┼──────┼──────┼──────┼──────┤
│ Win  │ Alt  │ Ctl  │ Sft  │      │          │  `   │  ~   │  [   │  ]   │  \   │
├──────┼──────┼──────┼──────┼──────┤          ├──────┼──────┼──────┼──────┼──────┤
│      │      │      │      │      │          │  ?   │  @   │  #   │  $   │  %   │
└──────┴──────┴──────┼──────┼──────┤          ├──────┼──────┼──────┴──────┴──────┘
                     │ hold │  ▽   │          │GrvEsc│  !   │
                     └──────┴──────┘          └──────┴──────┘
```

- Shifted symbols come from the left-hand Shift (index finger) on this layer:
  `_` = Sft+`-`, `+` = Sft+`=`, `{` `}` = Sft+`[` `]`, `|` = Sft+`\`.
- `GrvEsc` (`QK_GESC`) sends Esc normally, `~` with Shift, and `` ` `` with Win.
- `▽` on the left inner thumb means Space still works here.

### NUM: numbers (hold Backspace)

```
┌──────┬──────┬──────┬──────┬──────┐          ┌──────┬──────┬──────┬──────┬──────┐
│  1   │  2   │  3   │  4   │  5   │          │      │      │      │      │      │
├──────┼──────┼──────┼──────┼──────┤          ├──────┼──────┼──────┼──────┼──────┤
│  6   │  7   │  8   │  9   │  0   │          │      │ Sft  │ Ctl  │ Alt  │ Win  │
├──────┼──────┼──────┼──────┼──────┤          ├──────┼──────┼──────┼──────┼──────┤
│  ,   │  .   │  /   │  -   │  =   │          │      │      │      │      │      │
└──────┴──────┴──────┼──────┼──────┤          ├──────┼──────┼──────┴──────┴──────┘
                     │  '   │      │          │ hold │  ▽   │
                     └──────┴──────┘          └──────┴──────┘
```

`"` = Sft (right J) + `'`. The right outer thumb is transparent, so it still works as
Enter / MEDIA.

### MEDIA: media + system (hold Enter)

```
┌──────┬──────┬──────┬──────┬──────┐          ┌──────┬──────┬──────┬──────┬──────┐
│ BOOT │Mac/  │      │      │Brght+│          │      │      │      │      │      │
│      │ Win  │      │      │      │          │      │      │      │      │      │
├──────┼──────┼──────┼──────┼──────┤          ├──────┼──────┼──────┼──────┼──────┤
│      │      │      │      │Brght-│          │      │ Sft  │ Ctl  │ Alt  │ Win  │
├──────┼──────┼──────┼──────┼──────┤          ├──────┼──────┼──────┼──────┼──────┤
│ Prev │ Play │ Stop │ Next │ Mute │          │      │      │      │      │      │
└──────┴──────┴──────┼──────┼──────┤          ├──────┼──────┼──────┴──────┴──────┘
                     │ Vol- │ Vol+ │          │  ▽   │ hold │
                     └──────┴──────┘          └──────┴──────┘
```

- **BOOT** (`QK_BOOT`): reboot into the bootloader for flashing.
- **Mac/Win** (`CG_TOGG`): toggle Ctrl↔Cmd. See [§6](#6-macwin-mode).

### Where is…?

| Key | How to type it |
|---|---|
| Esc | `Q+W` combo, or SYM + right inner thumb |
| Alt+F4 | hold `S` (Alt) + `O+P` combo |
| `^` | NUM + Shift (right J) + `6` |
| `&` | NUM + Shift (right J) + `7` |
| `:` `"` `<` `>` `?` | left Shift (`F`) + `;` / `'`(NUM) / `,` / `.` / `/` |
| Delete, Home/End, PgUp/PgDn, F1–F12 (except F4), Caps Lock, PrintScreen, Insert | **not on the board** (see [§9](#9-further-suggestions-not-applied)) |

---

## 6. Mac/Win mode

One firmware covers both OSes. **Mac mode swaps Ctrl and GUI (Win/Cmd) on every key**,
home-row mods and the plain mod keys on layers included:

| Key | Windows mode | Mac mode |
|---|---|---|
| `A` / `;` hold (pinkies) | Win | **Ctrl** |
| `S` / `L` hold | Alt | Option (same key code, macOS calls it ⌥) |
| `D` / `K` hold (middle fingers) | Ctrl | **Cmd** |
| `F` / `J` hold | Shift | Shift |

So the Windows habit `D`+`C` (Ctrl+C) becomes Cmd+C on the Mac, with the same fingers.
Everything else keeps its **native macOS meaning**. That means on a Mac:
`D`+←/→ = Cmd+←/→ (line start/end), `S`+←/→ = Opt+←/→ (word jump),
`S`+Bksp = delete word, and `D`+Bksp = delete to line start.

**How the mode is chosen**

1. **Automatically at plug-in**: QMK's OS detection (`OS_DETECTION_ENABLE`) looks at
   how the host enumerates USB. About 250 ms after plug-in it switches to Mac mode for
   macOS/iOS and to Windows mode for Windows/Linux. If it can't tell (`OS_UNSURE`, which
   happens behind some KVMs/docks), it leaves the mode unchanged.
2. **Manually**: hold Enter (MEDIA) and press **W** (`CG_TOGG`). The new mode is saved in
   EEPROM and lasts until the next plug-in where detection gives a confident answer.

The logic is `process_detected_host_os_user()` at the bottom of `keymap.c`. Detection only
changes the mode in RAM, so plugging in never wears the EEPROM.

---

## 7. Behaviour & tuning

### Home-row mods / layer-taps (`config.h`)

| Setting | Value | Effect |
|---|---|---|
| `TAPPING_TERM` | 200 ms | Press longer than this = hold (mod/layer), shorter = tap |
| `PERMISSIVE_HOLD` | on | If another key is **pressed and released** while a mod-tap is down, it counts as a hold, even inside 200 ms. Makes `Ctrl(D)+C` fast; rolling `d→c` still types `dc` |
| `QUICK_TAP_TERM` | 0 | Tap-then-hold never auto-repeats the tap key, so you don't get accidental `ffff` |
| `QUICK_TAP_TERM_PER_KEY` | on | Exception: `get_quick_tap_term()` in `keymap.c` gives **Backspace** the full 200 ms, so it can repeat |

### Combos

`COMBO_ENABLE = yes`. Default `COMBO_TERM` is 50 ms: both keys have to go down within
50 ms of each other.

### Mouse keys

Accelerated mode, tuned faster than upstream. Each value in `config.h` is commented with
the current QMK default in brackets.

### Split

`SPLIT_USB_DETECT` + `SPLIT_USB_TIMEOUT 2500`: at boot, each half waits up to 2.5 s for USB
enumeration to decide whether it is master. This makes master detection reliable when the
TRRS cable powers the other Pro Micro.

### Firmware size

`LTO_ENABLE = yes` (link-time optimisation) brought the build from 92% to 68% of the
ATmega32U4's flash, even after adding OS detection. Keep an eye on the size line when you
add features. Over ~95% is a sign to drop something.

---

## 8. How to make common changes

Work loop: edit → `qmk compile` → flash the left half → test (e.g.
<https://config.qmk.fm/#/test>) → commit.

### Change a key

Every layer is a `LAYOUT_split_3x5_2(...)` with 34 entries in reading order:
10 top row, 10 home row, 10 bottom row (left 5 then right 5), then 4 thumbs
(left outer, left inner, right inner, right outer). Replace the keycode at that position.
Keycode list: <https://docs.qmk.fm/keycodes>.

### Add a layer

1. Add a name to `enum layers { BASE, NAV, SYM, NUM, MEDIA, FUN };`.
2. Add `[FUN] = LAYOUT_split_3x5_2( ... )` (copy a layer and fill in `KC_NO` / `KC_TRNS`).
3. Reach it from a key: `LT(FUN, KC_X)` (hold = layer, tap = key), `MO(FUN)` (hold only),
   `TG(FUN)` (toggle), `OSL(FUN)` (one-shot).

### Add a combo

```c
const uint16_t PROGMEM jk_combo[] = {SFT_J, CTL_K, COMBO_END};   // use the FULL keycode of mod-taps
combo_t key_combos[] = {
    COMBO(qw_esc, KC_ESC),
    COMBO(po_f4, KC_F4),
    COMBO(jk_combo, KC_DEL),
};
```

Combo keys must match exactly what is on the BASE layer: write `SFT_J`, not `KC_J`.

### Let another tap-hold key auto-repeat

Add a `case` to `get_quick_tap_term()`:

```c
case LT(NAV, KC_SPC):
    return TAPPING_TERM;
```

### Add a home-row-mod alias

Add a `#define` at the top of `keymap.c` (e.g. `#define CTL_X LCTL_T(KC_X)`) and use it in
the layer. Don't use `RALT_T` on Windows: Right Alt is AltGr.

### Enable a feature

Add `FEATURE_ENABLE = yes` to `rules.mk` (e.g. `CAPS_WORD_ENABLE = yes`) and its options
to `config.h`. Then check the firmware size line.

### Visualise the keymap

[keymap-drawer](https://github.com/caksoylar/keymap-drawer) draws SVGs from QMK keymaps:

```bash
qmk c2json -kb ferris/sweep -km rattatui ~/qmk_firmware/keyboards/ferris/sweep/keymaps/rattatui/keymap.c | keymap parse -q - > keymap.yaml && keymap draw keymap.yaml > keymap.svg
```

---

## 9. Further suggestions (not applied)

Already done: mouse keycode rename, Backspace repeat, Mac/Win mode, `QK_BOOT`, named
layers, config clean-up, LTO. These are ideas for later:

### Missing keys (Windows + programming)

- **Delete**, **Home/End/PgUp/PgDn**: on NAV around the arrows, or as combos (e.g. `J+K` = Del).
- **F1–F12**: F2 (rename), F5 (run/refresh), F10/F11 (step over/into in debuggers) and F12
  (go-to-definition, devtools) come up a lot when programming. The MEDIA layer's left hand
  has room for a 3×4 F-key block if the media keys move to the right hand.
- **Caps Word**: `CAPS_WORD_ENABLE = yes` + `#define BOTH_SHIFTS_TURNS_ON_CAPS_WORD`. Hold
  F+J to type `SCREAMING_SNAKE_CASE` until the next space.

### Home-row-mod quality of life

- **Lone Win/Alt press**: holding `A`/`;` too long and releasing without another key opens
  the Start menu (Alt does the same to menu bars). Fix with:
  ```c
  #define RETRO_TAPPING
  #define DUMMY_MOD_NEUTRALIZER_KEYCODE KC_F18
  #define MODS_TO_NEUTRALIZE { MOD_BIT(KC_LEFT_ALT), MOD_BIT(KC_LEFT_GUI), MOD_BIT(KC_RIGHT_GUI) }
  ```
- **Fast-typing misfires**: `#define FLOW_TAP_TERM 150` (a mod-tap pressed shortly after
  another key is always a tap). `#define CHORDAL_HOLD` (same-hand chords settle as taps)
  is even stronger, but then one-handed `D+C` copy needs the other hand's Ctrl or a longer hold.

### Combos

- `O+P → F4` sits on a common bigram ("st**op**", "p**o**int", "ex**po**rt"). A fast roll
  inside 50 ms fires F4. Either move F4 to a real F-key block, or limit the combo to
  when Alt is held:
  ```c
  // config.h: #define COMBO_SHOULD_TRIGGER
  bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
      if (combo_index == 1) return get_mods() & MOD_MASK_ALT;   // po_f4 only as Alt+F4
      return true;
  }
  ```

### Small layout nits

- NUM left inner thumb is `KC_NO`: `KC_TRNS` (Space) or `KC_DOT` would help with decimals.
- SYM: `KC_TILD` duplicates Shift+`` ` `` on a US layout, and `^`/`&` are only reachable
  through NUM+Shift. Swapping `~` for `^` or `&` would put more programming symbols on SYM.
- `QK_GESC` duplicates Esc and `` ` ``. That slot could hold `&`, `_`, `|` or Delete.

### Repo / workflow

- **QMK External Userspace** (<https://docs.qmk.fm/newbs_external_userspace>): the repo
  becomes `keyboards/ferris/sweep/keymaps/rattatui/…` + `qmk.json`, no symlink needed, and
  the bundled GitHub Actions workflow builds the `.hex` on every push. You can then flash
  from any machine with QMK Toolbox.
- Commit a keymap-drawer SVG so the diagrams can't drift from the code.

---

## 10. Troubleshooting

| Symptom | Likely cause / fix |
|---|---|
| Right half dead | TRRS cable loose or bad (they fail often). After a QMK update, reflash the right half too |
| Keys on one half come out mirrored or wrong | USB is in the right half while handedness is `MASTER_LEFT`. Plug into left, or set up `EE_HANDS` |
| `qmk flash` says `Resource busy` on `/dev/tty.usbmodem…` | macOS quirk in QMK's flash script. Nothing was written. Use the avrdude command in §4 |
| `qmk flash` waits forever | The half didn't enter the bootloader. Use MEDIA+Q, the reset button (double-tap on some clones) or bootmagic. Use a **data** USB cable, not a charge-only one |
| Cmd and Ctrl are the wrong way round | OS detection guessed wrong or couldn't tell (common through KVMs/docks). Press MEDIA+W to toggle |
| Mode reset to Windows after flashing with bootmagic | Bootmagic clears EEPROM. Detection or MEDIA+W sets it again |
| Backspace doesn't repeat | Tap once, then press **and hold** within 200 ms. A plain long hold is the NUM layer |
| Compile error: `'KC_…' undeclared` after a QMK update | A keycode was renamed. The compiler usually suggests the new name. Check the latest changelog |
| Board not detected after plugging in (only sometimes) | Some USB hubs/KVMs enumerate slowly. Raise `SPLIT_USB_TIMEOUT` |
| Unwanted Ctrl/Shift/Alt while typing | Home-row-mod misfire. See §9 (Flow Tap, Chordal Hold, longer `TAPPING_TERM`) |

---

## 11. References

- QMK keyboard definition: <https://github.com/qmk/qmk_firmware/tree/master/keyboards/ferris/sweep>
- Sweep hardware (incl. Half Swept): <https://github.com/davidphilipbarr/Sweep>
- Split keyboards (handedness, EE_HANDS, flashing): <https://docs.qmk.fm/features/split_keyboard>
- Tap-hold options: <https://docs.qmk.fm/tap_hold>
- OS detection: <https://docs.qmk.fm/features/os_detection>
- Magic keycodes (`CG_TOGG`): <https://docs.qmk.fm/keycodes_magic>
- Combos: <https://docs.qmk.fm/features/combo>
- Mouse keys: <https://docs.qmk.fm/features/mouse_keys>
- Breaking changes 2025-08-31 (mouse keycode removal): <https://docs.qmk.fm/ChangeLog/20250831>
- External userspace: <https://docs.qmk.fm/newbs_external_userspace>
- keymap-drawer: <https://github.com/caksoylar/keymap-drawer>
