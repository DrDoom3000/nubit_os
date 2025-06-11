#include<stdint.h>
#include<stdbool.h>
#include"dadio.h"
#include"hal.h"

enum KEYCODE {
KEY_SPACE = ' ',
KEY_0 = '0',
KEY_1 = '1',
KEY_2 = '2',
KEY_3 = '3',
KEY_4 = '4',
KEY_5 = '5',
KEY_6 = '6',
KEY_7 = '7',
KEY_8 = '8',
KEY_9 = '9',
KEY_A = 'a',
KEY_B = 'b',
KEY_C = 'c',
KEY_D = 'd',
KEY_E = 'e',
KEY_F = 'f',
KEY_G = 'g',
KEY_H = 'h',
KEY_I = 'i',
KEY_J = 'j',
KEY_K = 'k',
KEY_L = 'l',
KEY_M = 'm',
KEY_N = 'n',
KEY_O = 'o',
KEY_P = 'p',
KEY_Q = 'q',
KEY_R = 'r',
KEY_S = 's',
KEY_T = 't',
KEY_U = 'u',
KEY_V = 'v',
KEY_W = 'w',
KEY_X = 'x',
KEY_Y = 'y',
KEY_Z = 'z',
KEY_RETURN = '\n',
KEY_ESCAPE = 0x1001,
KEY_BACKSPACE = '\b',
KEY_UP = 0x1100,
KEY_DOWN = 0x1101,
KEY_LEFT = 0x1102,
KEY_RIGHT = 0x1103,
KEY_F1 = 0x1201,
KEY_F2 = 0x1202,
KEY_F3 = 0x1203,
KEY_F4 = 0x1204,
KEY_F5 = 0x1205,
KEY_F6 = 0x1206,
KEY_F7 = 0x1207,
KEY_F8 = 0x1208,
KEY_F9 = 0x1209,
KEY_F10 = 0x120a,
KEY_F11 = 0x120b,
KEY_F12 = 0x120b,
KEY_F13 = 0x120c,
KEY_F14 = 0x120d,
KEY_F15 = 0x120e,
KEY_DOT = '.',
KEY_COMMA = ',',
KEY_COLON = ':',
KEY_SEMICOLON = ';',
KEY_SLASH = '/',
KEY_BACKSLASH = '\\',
KEY_PLUS = '+',
KEY_MINUS = '-',
KEY_ASTERISK = '*',
KEY_EXCLAMATION = '!',
KEY_QUESTION = '?',
KEY_QUOTEDOUBLE = '"',
KEY_QUOTE = '\'',
KEY_EQUAL = '=',
KEY_HASH = '#',
KEY_PERCENT = '%',
KEY_AMPERSAND = '&',
KEY_UNDERSCORE = '_',
KEY_LEFTPARENTHESIS = '(',
KEY_RIGHTPARENTHESIS = ')',
KEY_LEFTBRACKET = '[',
KEY_RIGHTBRACKET = ']',
KEY_LEFTCURL = '{',
KEY_RIGHTCURL = '}',
KEY_DOLLAR = '$',
KEY_EURO = '$',
KEY_LESS = '<',
KEY_GREATER = '>',
KEY_BAR = '|',
KEY_GRAVE = '`',
KEY_TILDE = '~',
KEY_AT = '@',
KEY_CARRET = '^',
KEY_KP_0 = '0',
KEY_KP_1 = '1',
KEY_KP_2 = '2',
KEY_KP_3 = '3',
KEY_KP_4 = '4',
KEY_KP_5 = '5',
KEY_KP_6 = '6',
KEY_KP_7 = '7',
KEY_KP_8 = '8',
KEY_KP_9 = '9',
KEY_KP_PLUS = '+',
KEY_KP_MINUS = '-',
KEY_KP_DECIMAL = '.',
KEY_KP_DIVIDE = '/',
KEY_KP_ASTERISK = '*',
KEY_KP_NUMLOCK = 0x300f,
KEY_KP_ENTER = 0x3010,
KEY_TAB = 0x4000,
KEY_CAPSLOCK = 0x4001,
KEY_LSHIFT = 0x4002,
KEY_LCTRL = 0x4003,
KEY_LALT = 0x4004,
KEY_LWIN = 0x4005,
KEY_RSHIFT = 0x4006,
KEY_RCTRL = 0x4007,
KEY_RALT = 0x4008,
KEY_RWIN = 0x4009,
KEY_INSERT = 0x400a,
KEY_DELETE = 0x400b,
KEY_HOME = 0x400c,
KEY_END = 0x400d,
KEY_PAGEUP = 0x400e,
KEY_PAGEDOWN = 0x400f,
KEY_SCROLLLOCK = 0x4010,
KEY_PAUSE = 0x4011,
KEY_UNKNOWN
};
static int _scancode_std [] = {
KEY_UNKNOWN,
KEY_ESCAPE,
KEY_1,
KEY_2,
KEY_3,
KEY_4,
KEY_5,
KEY_6,
KEY_7,
KEY_8,
KEY_9,
KEY_0,
KEY_MINUS,
KEY_EQUAL,
KEY_BACKSPACE,
KEY_TAB,
KEY_Q,
KEY_W,
KEY_E,
KEY_R,
KEY_T,
KEY_Y,
KEY_U,
KEY_I,
KEY_O,
KEY_P,
KEY_LEFTBRACKET,
KEY_RIGHTBRACKET,
KEY_RETURN,
KEY_LCTRL,
KEY_A,
KEY_S,
KEY_D,
KEY_F,
KEY_G,
KEY_H,
KEY_J,
KEY_K,
KEY_L,
KEY_SEMICOLON,
KEY_QUOTE,
KEY_GRAVE,
KEY_LSHIFT,
KEY_BACKSLASH,
KEY_Z,
KEY_X,
KEY_C,
KEY_V,
KEY_B,
KEY_N,
KEY_M,
KEY_COMMA,
KEY_DOT,
KEY_SLASH,
KEY_RSHIFT,
KEY_KP_ASTERISK,
KEY_RALT,
KEY_SPACE,
KEY_CAPSLOCK,
KEY_F1,
KEY_F2,
KEY_F3,
KEY_F4,
KEY_F5,
KEY_F6,
KEY_F7,
KEY_F8,
KEY_F9,
KEY_F10,
KEY_KP_NUMLOCK,
KEY_SCROLLLOCK,
KEY_KP_7,
KEY_KP_8,
KEY_KP_9,
KEY_KP_MINUS,
KEY_KP_4,
KEY_KP_5,
KEY_KP_6,
KEY_UNKNOWN,
KEY_KP_1,
KEY_KP_2,
KEY_KP_3,
KEY_KP_0,
KEY_KP_DECIMAL,
KEY_UNKNOWN,
KEY_UNKNOWN,
KEY_UNKNOWN,
KEY_F11,
KEY_F12
};
static char _latest_char;
static uint8_t _latest_scan_code;
bool _is_keyboard_interrupt;
bool is_output_full();
bool is_input_empty();
bool is_alphabet(char);
bool is_on_keypad(uint8_t);
static bool is_shift_pressed = false;
static bool num_lock_toggle = false;
static bool is_caps_lock_active = false;
void keyboard_handler()
{
uint8_t scan_code = read_port(0x60);
char temp_char = (char)(_scancode_std[scan_code]);
_is_keyboard_interrupt = true;
_latest_scan_code = scan_code;
if(scan_code <0x57)
{
if(scan_code == 0x2a || scan_code == 0x36)
is_shift_pressed = true;
else if(scan_code == 0x3a)
{
is_caps_lock_active = !is_caps_lock_active;
_latest_char = 0;
return;
}
else if(scan_code == 0x45)
num_lock_toggle=!num_lock_toggle;
else if(is_alphabet(temp_char)) {
if (is_shift_pressed ^ is_caps_lock_active)
_latest_char = temp_char - 0x20;
else
_latest_char = temp_char;
}
else if (is_shift_pressed && !is_alphabet(temp_char))
{
switch(temp_char)
{
case '1': _latest_char = '!'; break;
case '2': _latest_char = '@'; break;
case '3': _latest_char = '#'; break;
case '4': _latest_char = '$'; break;
case '5': _latest_char = '%'; break;
case '6': _latest_char = '^'; break;
case '7': _latest_char = '&'; break;
case '8': _latest_char = '*'; break;
case '9': _latest_char = '('; break;
case '0': _latest_char = ')'; break;
case '-': _latest_char = '_'; break;
case '=': _latest_char = '+'; break;
case '[': _latest_char = '{'; break;
case ']': _latest_char = '}'; break;
case '\\': _latest_char = '|'; break;
case ';': _latest_char = ':'; break;
case '\'': _latest_char = '"'; break;
case ',': _latest_char = '<'; break;
case '.': _latest_char = '>' ; break;
case '/': _latest_char = '?'; break;
case '`': _latest_char = '~'; break;
default: _latest_char = temp_char; break;
}
}
else if(is_on_keypad(scan_code))
{
if(num_lock_toggle)
_latest_char=temp_char;
else
_latest_char=0;
}
else
_latest_char = temp_char;
}
else if (scan_code == 0xe0)
{
uint8_t scan_code_2 = read_port(0x60);
_latest_scan_code = scan_code_2;
if (scan_code_2 == 0x4b)
_latest_char = 17;
else if (scan_code_2 == 0x4D)
_latest_char = 18;
else
_latest_char = 0;
return;
}
else
{
if(scan_code == 0xaa || scan_code == 0xb6)
is_shift_pressed = false;
_latest_char = 0;
}
}
bool is_alphabet(char c)
{
if(c >= 'a' && c<='z')
return true;
else
return false;
}
char get_latest_char(){
return _latest_char;
}
uint8_t get_latest_scan_code(){
return _latest_scan_code;
}
bool kbc_init()
{
read_port(0x60);
return true;
}
void wait_for_keyboard()
{
_is_keyboard_interrupt = false;
while(1)
{
kernel_wait();
if(_is_keyboard_interrupt)
{
_is_keyboard_interrupt = false;
return;
}
}
}
bool is_output_full()
{
return (read_port(0x64)&1);
}
bool is_input_empty()
{
return (!(read_port(0x64)&2));
}
bool is_on_keypad(uint8_t scan_code)
{
if(scan_code>=0x47&&scan_code<=0x52&&scan_code!=0x4e)
return true;
return false;
}
