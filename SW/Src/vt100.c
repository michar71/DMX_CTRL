#include <stdbool.h>
#include <string.h>
#include "vt100.h"

static const uint8_t key_cursor_up_seq[]      = {0x1B, 0x5B, 0x41};
static const uint8_t key_cursor_down_seq[]    = {0x1B, 0x5B, 0x42};
static const uint8_t key_cursor_left_seq[]    = {0x1B, 0x5B, 0x44};
static const uint8_t key_cursor_right_seq[]   = {0x1B, 0x5B, 0x43};
static const uint8_t key_tab_seq[]            = {0x09};
static const uint8_t key_backspace_seq[]      = {0x08};
static const uint8_t key_delete_seq[]         = {0x7F};
static const uint8_t key_clear_eol[]          = {0x1B, 0x5B, 0x4B};
static const uint8_t key_clear_entire_line[]  = {0x1B, 0x5B, 0x32, 0x4B};
static const uint8_t key_move_cursor_left[]   = {0x1B, 0x5B, 0x01, 0x44};
static const uint8_t key_clear_screen[]       = {0x1B, 0x5B, 0x32, 0x4A};
static const uint8_t key_move_cursor_home[]   = {0x1B, 0x5B, 0x48};

static const vt100_keycode_t vt100_keycodes[VT100_KEY_MAX] = 
{
    {VT100_KEY_CURSOR_UP,           (uint8_t*)key_cursor_up_seq,         3},
    {VT100_KEY_CURSOR_DOWN,         (uint8_t*)key_cursor_down_seq,       3},
    {VT100_KEY_CURSOR_LEFT,         (uint8_t*)key_cursor_left_seq,       3},
    {VT100_KEY_CURSOR_RIGHT,        (uint8_t*)key_cursor_right_seq,      3},
    {VT100_KEY_TAB,                 (uint8_t*)key_tab_seq,               1},
    {VT100_KEY_BACKSPACE,           (uint8_t*)key_backspace_seq,         1},
    {VT100_KEY_DELETE,              (uint8_t*)key_delete_seq,            1},
    {VT100_KEY_CLEAR_EOL,           (uint8_t*)key_clear_eol,             sizeof(key_clear_eol)},
    {VT100_KEY_CLEAR_ENTIRE_LINE,   (uint8_t*)key_clear_entire_line,     sizeof(key_clear_entire_line)},
    {VT100_KEY_MOVE_CURSOR_LEFT,    (uint8_t*)key_move_cursor_left,      sizeof(key_move_cursor_left)},
    {VT100_KEY_CLEAR_SCREEN,        (uint8_t*)key_clear_screen,          sizeof(key_clear_screen)},
    {VT100_KEY_MOVE_CURSOR_HOME,    (uint8_t*)key_move_cursor_home,      sizeof(key_move_cursor_home)},
};

static bool is_printable(uint8_t byte)
{
    return (byte >= 0x20 && byte <= 0x7E)? true:false;
}

static vt100_key_t find_key(uint8_t * buf, uint8_t len)
{
    for (unsigned int ii = 0; ii < sizeof(vt100_keycodes)/sizeof(vt100_keycode_t); ++ii)
    {
        if ((len == vt100_keycodes[ii].code_len) &&
            (0 == strncmp((const char*)vt100_keycodes[ii].code, (const char*)buf, len)))
        {
            return vt100_keycodes[ii].key;
        }
    }
    
    return VT100_KEY_INVALID;
}

vt100_key_t vt100_process_byte(uint8_t byte)
{
    static uint8_t cbuf[8] = {0};
    static uint8_t cbuf_idx = 0;
    
    if (0 == cbuf_idx && true == is_printable(byte))
    {
        return VT100_KEY_PRINTABLE;
    }
    else if (0x0D == byte) /* Enter */
    {
        return VT100_KEY_ENTER;
    }
    else
    {
        vt100_key_t key = VT100_KEY_INVALID;
        cbuf[cbuf_idx++] = byte;
        key = find_key(cbuf, cbuf_idx);
        if (VT100_KEY_INVALID != key)
        {
            cbuf_idx = 0;
            memset(cbuf, 0, sizeof(cbuf));
            return key;
        }
        else
        {
            if (cbuf_idx >= sizeof(cbuf))
            {
                cbuf_idx = 0;
                memset(cbuf, 0, sizeof(cbuf));
            }
        }
    }

    return VT100_KEY_INVALID;
}

vt100_keycode_t * vt100_get_keycode(vt100_key_t key)
{
    if (key >= VT100_KEY_MAX) return NULL;
    
    for (unsigned int ii = 0; ii < sizeof(vt100_keycodes)/sizeof(vt100_keycode_t); ++ii)
    {
        if (vt100_keycodes[ii].key == key)
        {
            return (vt100_keycode_t*)&vt100_keycodes[ii];
        }
    }
    
    return NULL;
}
