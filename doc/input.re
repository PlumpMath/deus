//
// Compile this file with the following command:
//
//   re2c -b -i --no-generation-date --no-version -o input.cpp input.re
//

#include <js/input.h>

namespace js {

//
// https://w3c.github.io/uievents-code
//

key parse_key(const char* code) noexcept {
  const char* YYCURSOR = code;
  const char* YYMARKER = YYCURSOR;

  /*!re2c

  re2c:define:YYCTYPE = "char";
  re2c:yyfill:enable = 0;

  'Space'           { return key::space; }
  'Quote'           { return key::quote; }
  'Comma'           { return key::comma; }
  'Minus'           { return key::minus; }
  'Period'          { return key::period; }
  'Slash'           { return key::slash; }
  'Digit0'          { return key::digit_0; }
  'Digit1'          { return key::digit_1; }
  'Digit2'          { return key::digit_2; }
  'Digit3'          { return key::digit_3; }
  'Digit4'          { return key::digit_4; }
  'Digit5'          { return key::digit_5; }
  'Digit6'          { return key::digit_6; }
  'Digit7'          { return key::digit_7; }
  'Digit8'          { return key::digit_8; }
  'Digit9'          { return key::digit_9; }
  'Semicolon'       { return key::semicolon; }
  'Equal'           { return key::equal; }
  'KeyA'            { return key::a; }
  'KeyB'            { return key::b; }
  'KeyC'            { return key::c; }
  'KeyD'            { return key::d; }
  'KeyE'            { return key::e; }
  'KeyF'            { return key::f; }
  'KeyG'            { return key::g; }
  'KeyH'            { return key::h; }
  'KeyI'            { return key::i; }
  'KeyJ'            { return key::j; }
  'KeyK'            { return key::k; }
  'KeyL'            { return key::l; }
  'KeyM'            { return key::m; }
  'KeyN'            { return key::n; }
  'KeyO'            { return key::o; }
  'KeyP'            { return key::p; }
  'KeyQ'            { return key::q; }
  'KeyR'            { return key::r; }
  'KeyS'            { return key::s; }
  'KeyT'            { return key::t; }
  'KeyU'            { return key::u; }
  'KeyV'            { return key::v; }
  'KeyW'            { return key::w; }
  'KeyX'            { return key::x; }
  'KeyY'            { return key::y; }
  'KeyZ'            { return key::z; }
  'BracketLeft'     { return key::bracket_left; }
  'Backslash'       { return key::backslash; }
  'BracketRight'    { return key::bracket_right; }
  'Backquote'       { return key::backquote; }
  'Lang1'           { return key::lang_1; }
  'Lang2'           { return key::lang_2; }
  'Escape'          { return key::escape; }
  'Enter'           { return key::enter; }
  'Tab'             { return key::tab; }
  'Backspace'       { return key::backspace; }
  'Insert'          { return key::insert; }
  'Delete'          { return key::del; }
  'ArrowRight'      { return key::right; }
  'ArrowLeft'       { return key::left; }
  'ArrowDown'       { return key::down; }
  'ArrowUp'         { return key::up; }
  'PageUp'          { return key::page_up; }
  'PageDown'        { return key::page_down; }
  'Home'            { return key::home; }
  'End'             { return key::end; }
  'CapsLock'        { return key::caps_lock; }
  'ScrollLock'      { return key::scroll_lock; }
  'NumLock'         { return key::num_lock; }
  'PrintScreen'     { return key::print_screen; }
  'Pause'           { return key::pause; }
  'F1'              { return key::f1; }
  'F2'              { return key::f2; }
  'F3'              { return key::f3; }
  'F4'              { return key::f4; }
  'F5'              { return key::f5; }
  'F6'              { return key::f6; }
  'F7'              { return key::f7; }
  'F8'              { return key::f8; }
  'F9'              { return key::f9; }
  'F10'             { return key::f10; }
  'F11'             { return key::f11; }
  'F12'             { return key::f12; }
  'F13'             { return key::f13; }
  'F14'             { return key::f14; }
  'F15'             { return key::f15; }
  'F16'             { return key::f16; }
  'F17'             { return key::f17; }
  'F18'             { return key::f18; }
  'F19'             { return key::f19; }
  'F20'             { return key::f20; }
  'F21'             { return key::f21; }
  'F22'             { return key::f22; }
  'F23'             { return key::f23; }
  'F24'             { return key::f24; }
  'F25'             { return key::f25; }
  'Numpad0'         { return key::numpad_0; }
  'Numpad1'         { return key::numpad_1; }
  'Numpad2'         { return key::numpad_2; }
  'Numpad3'         { return key::numpad_3; }
  'Numpad4'         { return key::numpad_4; }
  'Numpad5'         { return key::numpad_5; }
  'Numpad6'         { return key::numpad_6; }
  'Numpad7'         { return key::numpad_7; }
  'Numpad8'         { return key::numpad_8; }
  'Numpad9'         { return key::numpad_9; }
  'NumpadDecimal'   { return key::numpad_decimal; }
  'NumpadDivide'    { return key::numpad_divide; }
  'NumpadMultiply'  { return key::numpad_multiply; }
  'NumpadSubtract'  { return key::numpad_subtract; }
  'NumpadAdd'       { return key::numpad_add; }
  'NumpadEnter'     { return key::numpad_enter; }
  'NumpadEqual'     { return key::numpad_equal; }
  'ShiftLeft'       { return key::shift_left; }
  'ControlLeft'     { return key::control_left; }
  'AltLeft'         { return key::alt_left; }
  'MetaLeft'        { return key::meta_left; }
  'ShiftRight'      { return key::shift_right; }
  'ControlRight'    { return key::control_right; }
  'AltRight'        { return key::alt_right; }
  'MetaRight'       { return key::meta_right; }
  'Menu'            { return key::menu; }

  */

  return key::none;
}

}  // namespace js