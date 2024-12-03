#include QMK_KEYBOARD_H

// Left-hand home row mods
#define GUI_A LGUI_T(KC_A)
#define ALT_S LALT_T(KC_S)
#define CTL_D LCTL_T(KC_D)
#define SFT_F LSFT_T(KC_F)

// Right-hand home row mods
#define SFT_J RSFT_T(KC_J)
#define CTL_K RCTL_T(KC_K)
#define ALT_L LALT_T(KC_L)
#define GUI_SCLN RGUI_T(KC_SCLN)

// Tap Q and Hold Esc
const uint16_t PROGMEM qw_esc[] = {KC_Q, KC_W, COMBO_END};
combo_t key_combos[] = {
    COMBO(qw_esc, KC_ESC),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_split_3x5_2(
        KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,             KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
        GUI_A, ALT_S, CTL_D, SFT_F, KC_G, KC_H, SFT_J, CTL_K, ALT_L, GUI_SCLN,
        KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,             KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
                             LT(2, KC_TAB), LT(1, KC_SPC),    LT(3, KC_BSPC), LT(4, KC_ENT)
    ),
    [1] = LAYOUT_split_3x5_2(
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,            KC_ACL2, KC_WH_L, KC_WH_D, KC_WH_U, KC_WH_R,
        KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, KC_NO,            KC_ACL1, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,            KC_ACL0, KC_MS_L, KC_MS_D, KC_MS_U, KC_MS_R,
                             KC_TRNS, KC_TRNS,                KC_BTN1, KC_BTN2
    ),
    [2] = LAYOUT_split_3x5_2(
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,            KC_ASTR, KC_LPRN, KC_RPRN, KC_MINS, KC_EQL,
        KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, KC_NO,            KC_TILD, KC_CIRC, KC_LBRC, KC_RBRC, KC_BSLS,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,            KC_QUES, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,
                             KC_TRNS, KC_TRNS,                QK_GESC, KC_EXLM
    ),
    [3] = LAYOUT_split_3x5_2(
        KC_1,    KC_2,    KC_3,    KC_4,    KC_5,             KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_6,    KC_7,    KC_8,    KC_9,    KC_0,             KC_NO,   KC_LSFT, KC_LCTL, KC_LALT, KC_LGUI,
        KC_COMM, KC_DOT,  KC_SLSH, KC_MINS, KC_EQL,           KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
                                KC_QUOT,  KC_NO,             KC_ASTR, KC_TRNS
    ),
    [4] = LAYOUT_split_3x5_2(
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_BRIU,          KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_BRID,          KC_NO,   KC_LSFT, KC_LCTL, KC_LALT, KC_LGUI,
        KC_MPRV, KC_MPLY, KC_MSTP, KC_MNXT, KC_MUTE,          KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
                             KC_VOLD, KC_VOLU,               KC_TRNS, KC_TRNS
    )
};
