/*
Copyright 2020 Pierre Chevalier <pierrechevalier83@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
// Mouse keys (accelerated mode), tuned for a comfortable speed/accuracy trade-off
// assuming a screen refresh rate of 60 Hz or higher. QMK defaults in brackets.
// Repeat interval in ms [20]: lower = faster, smoother cursor.
#define MOUSEKEY_INTERVAL 16
// Time (in intervals) to reach max speed [30]: longer ramp keeps short moves precise.
#define MOUSEKEY_TIME_TO_MAX 40
// Delay before the cursor starts repeating, in ms [10].
#define MOUSEKEY_DELAY 100
// Same delay for the mouse wheel [10].
#define MOUSEKEY_WHEEL_DELAY 100
// Wheel repeat interval in ms [80].
#define MOUSEKEY_WHEEL_INTERVAL 50
// Wheel time (in intervals) to reach max speed [40].
#define MOUSEKEY_WHEEL_TIME_TO_MAX 100

// Pick good defaults for enabling homerow modifiers
#define TAPPING_TERM 200
#define PERMISSIVE_HOLD
#define QUICK_TAP_TERM 0
// Re-enable tap-then-hold auto-repeat for selected keys (see get_quick_tap_term in keymap.c)
#define QUICK_TAP_TERM_PER_KEY

// Split: wait up to 2.5 s for USB enumeration to decide which half is master
#define SPLIT_USB_DETECT
#define SPLIT_USB_TIMEOUT 2500
