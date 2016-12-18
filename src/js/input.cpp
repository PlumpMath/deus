#include <js/input.h>
#include <string_view>

namespace js {

#ifdef __EMSCRIPTEN__

namespace {

inline key parse_2(const std::string_view& name) noexcept {
  switch (name[0]) {
  case 'F':
    switch (name[1]) {
    case '1': return key::f1;
    case '2': return key::f2;
    case '3': return key::f3;
    case '4': return key::f4;
    case '5': return key::f5;
    case '6': return key::f6;
    case '7': return key::f7;
    case '8': return key::f8;
    case '9': return key::f9;
    }
    break;  // 'F'
  }
  return key::none;
}

inline key parse_3(const std::string_view& name) noexcept {
  switch (name[0]) {
  case 'F':
    switch (name[1]) {
    case '1':
      switch (name[2]) {
      case '0': return key::f10;
      case '1': return key::f11;
      case '2': return key::f12;
      case '3': return key::f13;
      case '4': return key::f14;
      case '5': return key::f15;
      case '6': return key::f16;
      case '7': return key::f17;
      case '8': return key::f18;
      case '9': return key::f19;
      }
      break;  // '1'
    case '2':
      switch (name[2]) {
      case '0': return key::f20;
      case '1': return key::f21;
      case '2': return key::f22;
      case '3': return key::f23;
      case '4': return key::f24;
      case '5': return key::f25;
      }
      break;  // '2'
    }
    break;  // 'F'
  case 'E': return key::end;
  case 'T': return key::tab;
  }
  return key::none;
}

inline key parse_4(const std::string_view& name) noexcept {
  switch (name[0]) {
  case 'K':
    switch (name[3]) {
    case 'A': return key::a;
    case 'B': return key::b;
    case 'C': return key::c;
    case 'D': return key::d;
    case 'E': return key::e;
    case 'F': return key::f;
    case 'G': return key::g;
    case 'H': return key::h;
    case 'I': return key::i;
    case 'J': return key::j;
    case 'K': return key::k;
    case 'L': return key::l;
    case 'M': return key::m;
    case 'N': return key::n;
    case 'O': return key::o;
    case 'P': return key::p;
    case 'Q': return key::q;
    case 'R': return key::r;
    case 'S': return key::s;
    case 'T': return key::t;
    case 'U': return key::u;
    case 'V': return key::v;
    case 'W': return key::w;
    case 'X': return key::x;
    case 'Y': return key::y;
    case 'Z': return key::z;
    }
    break;  // 'K'
  case 'H': return key::home;
  case 'M': return key::menu;
  }
  return key::none;
}

inline key parse_5(const std::string_view& name) noexcept {
  switch (name[0]) {
  case 'C': return key::comma;
  case 'E':
    switch (name[1]) {
    case 'q': return key::equal;
    case 'n': return key::enter;
    }
    break;  // 'E'
  case 'L':
    switch (name[4]) {
    case '1': return key::lang_1;
    case '2': return key::lang_2;
    }
    break;  // 'L'
  case 'M': return key::minus;
  case 'P': return key::pause;
  case 'Q': return key::quote;
  case 'S':
    switch (name[1]) {
    case 'p': return key::space;
    case 'l': return key::slash;
    }
    break;  // 'S'
  }
  return key::none;
}

inline key parse_6(const std::string_view& name) noexcept {
  switch (name[0]) {
  case 'D':
    switch (name[1]) {
    case 'e': return key::del;
    case 'i':
      switch (name[5]) {
      case '0': return key::digit_0;
      case '1': return key::digit_1;
      case '2': return key::digit_2;
      case '3': return key::digit_3;
      case '4': return key::digit_4;
      case '5': return key::digit_5;
      case '6': return key::digit_6;
      case '7': return key::digit_7;
      case '8': return key::digit_8;
      case '9': return key::digit_9;
      }
      break;  // 'i'
    }
    break;  // 'D'
  case 'E': return key::escape;
  case 'I': return key::insert;
  case 'P':
    switch (name[1]) {
    case 'a': return  key::page_up;
    case 'e': return  key::period;
    }
    break;  // 'P'
  }
  return key::none;
}

inline key parse_7(const std::string_view& name) noexcept {
  switch (name[0]) {
  case 'A':
    switch (name[1]) {
    case 'l': return key::alt_left;
    case 'r': return key::up;
    }
    break;  // 'A'
  case 'N':
    if (name[3]) {
    case 'L': return key::num_lock;
    case 'p':
      switch (name[6]) {
      case '0': return key::numpad_0;
      case '1': return key::numpad_1;
      case '2': return key::numpad_2;
      case '3': return key::numpad_3;
      case '4': return key::numpad_4;
      case '5': return key::numpad_5;
      case '6': return key::numpad_6;
      case '7': return key::numpad_7;
      case '8': return key::numpad_8;
      case '9': return key::numpad_9;
      }
      break;  // 'p'
    }
    break;  // 'N'
  }
  return key::none;
}

inline key parse_8(const std::string_view& name) noexcept {
  switch (name[0]) {
  case 'A': return key::alt_right;
  case 'C': return key::caps_lock;
  case 'M': return key::meta_left;
  case 'P': return key::page_down;
  }
  return key::none;
}

inline key parse_9(const std::string_view& name) noexcept {
  switch (name[0]) {
  case 'A':
    switch (name[5]) {
    case 'L': return key::left;
    case 'D': return key::down;
    }
    break;  // 'A'
  case 'B':
    switch (name[5]) {
    case 'l': return key::backslash;
    case 'u': return key::backquote;
    case 'p': return key::backspace;
    }
    break;  // 'B'
  case 'M': return key::meta_right;
  case 'N': return key::numpad_add;
  case 'S':
    switch (name[1]) {
    case 'e': return key::semicolon;
    case 'h': return key::shift_left;
    }
    break;  // 'S'
  }
  return key::none;
}

inline key parse_10(const std::string_view& name) noexcept {
  switch (name[0]) {
  case 'A': return key::right;
  case 'S':
    switch (name[1]) {
    case 'c': return key::scroll_lock;
    case 'h': return key::shift_right;
    }
    break;  // 'S'
  }
  return key::none;
}

inline key parse_11(const std::string_view& name) noexcept {
  switch (name[0]) {
  case 'B': return key::bracket_left;
  case 'C': return key::control_left;
  case 'N':
    switch (name[7]) {
    case 'n': return key::numpad_enter;
    case 'q': return key::numpad_equal;
    }
    break;  // 'N'
  case 'P': return key::print_screen;
  }
  return key::none;
}

inline key parse_12(const std::string_view& name) noexcept {
  switch (name[0]) {
  case 'B': return key::bracket_right;
  case 'C': return key::control_right;
  case 'N': return key::numpad_divide;
  }
  return key::none;
}

inline key parse_13() noexcept {
  return key::numpad_decimal;
}

inline key parse_14(const std::string_view& name) noexcept {
  switch (name[6]) {
  case 'M': return key::numpad_multiply;
  case 'S': return key::numpad_subtract;
  }
  return key::none;
}

}  // namespace

key parse_key(const char* code) noexcept {
  const std::string_view name(code);
  const auto size = name.size();
  switch (size) {
    case 2: return parse_2(code);
    case 3: return parse_3(code);
    case 4: return parse_4(code);
    case 5: return parse_5(code);
    case 6: return parse_6(code);
    case 7: return parse_7(code);
    case 8: return parse_8(code);
    case 9: return parse_9(code);
    case 10: return parse_10(code);
    case 11: return parse_11(code);
    case 12: return parse_12(code);
    case 13: return parse_13();;
    case 14: return parse_14(code);
  }
  return key::none;
}

#endif

}  // namespace js