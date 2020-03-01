#ifndef VT100_H
#define VT100_H

#include <stdint.h>

typedef enum
{
    VT100_KEY_INVALID,
    VT100_KEY_PRINTABLE,
    VT100_KEY_CURSOR_UP,
    VT100_KEY_CURSOR_DOWN,
    VT100_KEY_CURSOR_LEFT,
    VT100_KEY_CURSOR_RIGHT,
    VT100_KEY_TAB,
    VT100_KEY_BACKSPACE,
    VT100_KEY_ENTER,
    VT100_KEY_DELETE,
    VT100_KEY_CLEAR_EOL,
    VT100_KEY_CLEAR_ENTIRE_LINE,
    VT100_KEY_MOVE_CURSOR_LEFT,
    VT100_KEY_CLEAR_SCREEN,
    VT100_KEY_MOVE_CURSOR_HOME,
    VT100_KEY_MAX
} vt100_key_t;

typedef struct
{
    vt100_key_t     key;
    uint8_t *       code;
    uint8_t         code_len;
} vt100_keycode_t;

extern vt100_key_t vt100_process_byte(uint8_t byte);
extern vt100_keycode_t * vt100_get_keycode(vt100_key_t key);

#endif
