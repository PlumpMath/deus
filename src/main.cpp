#include <client.h>
#include <js/log.h>
#include <exception>
#include <iterator>
#include <memory>
#include <stdexcept>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#else
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <resource.h>
#include <iostream>
#include <utf8.h>
#endif

constexpr auto scroll_line_factor = 100.0;
constexpr auto scroll_page_factor = 400.0;

#ifdef __EMSCRIPTEN__
extern "C" void show_canvas();
#else
inline void show_canvas() {}
#endif

class canvas {
public:
  canvas() {
#ifndef __EMSCRIPTEN__
    glfwSetErrorCallback([](int ec, const char* message) {
      throw std::runtime_error(message);
    });
    if (!glfwInit()) {
      throw std::runtime_error("Could not initialize GLFW.");
    }
#endif
  }

  ~canvas() {
    client_.reset();
#ifdef __EMSCRIPTEN__
    if (handle_) {
      emscripten_webgl_make_context_current(handle_);
      emscripten_webgl_destroy_context(handle_);
    }
#else
    if (handle_) {
      glfwMakeContextCurrent(nullptr);
      glfwDestroyWindow(handle_);
    }
    glfwTerminate();
#endif
  }

  void create(const char* id = nullptr) {
#ifdef __EMSCRIPTEN__
    double cx = 800.0;
    double cy = 600.0;
    if (emscripten_get_element_css_size(id, &cx, &cy) != EMSCRIPTEN_RESULT_SUCCESS) {
      throw std::runtime_error("Could not get canvas size.");
    }
    emscripten_set_canvas_size(static_cast<int>(cx), static_cast<int>(cy));
    EmscriptenWebGLContextAttributes attributes = {};
    emscripten_webgl_init_context_attributes(&attributes);
    attributes.alpha = EM_FALSE;
    attributes.depth = EM_TRUE;
    attributes.stencil = EM_TRUE;
    attributes.antialias = EM_TRUE;
    attributes.premultipliedAlpha = EM_FALSE;
    attributes.preserveDrawingBuffer = EM_FALSE;
    attributes.preferLowPowerToHighPerformance = EM_TRUE;
    attributes.majorVersion = 2;
    attributes.minorVersion = 0;
    attributes.enableExtensionsByDefault = EM_FALSE;
    handle_ = emscripten_webgl_create_context(id, &attributes);
    if (!handle_) {
      throw std::runtime_error("Could not create WebGL 2 cnotext.");
    }
    if (emscripten_webgl_make_context_current(handle_) != EMSCRIPTEN_RESULT_SUCCESS) {
      throw std::runtime_error("Could not initialize WebGL 2.");
    }
    glGetError();
#else
    auto cx = 1024;
    auto cy = 768;
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 0);
    glfwWindowHint(GLFW_DEPTH_BITS, 16);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_ACCUM_RED_BITS, 0);
    glfwWindowHint(GLFW_ACCUM_GREEN_BITS, 0);
    glfwWindowHint(GLFW_ACCUM_BLUE_BITS, 0);
    glfwWindowHint(GLFW_ACCUM_ALPHA_BITS, 0);
    glfwWindowHint(GLFW_AUX_BUFFERS, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    handle_ = glfwCreateWindow(cx, cy, "DEUS", nullptr, nullptr);
    if (!handle_) {
      throw std::runtime_error("Could not create OpenGL ES 3.0 cnotext.");
    }

    auto hwnd = glfwGetWin32Window(handle_);
    auto icon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_MAIN));
    SendMessage(hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon));

    glfwSetInputMode(handle_, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
    glfwSetInputMode(handle_, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetWindowUserPointer(handle_, this);
    glfwMakeContextCurrent(handle_);
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
      throw std::runtime_error("Could not initialize GLEW.");
    }
#endif

    client_ = std::make_unique<client>(static_cast<GLsizei>(cx), static_cast<GLsizei>(cy));

#ifdef __EMSCRIPTEN__
    static auto get = [](void* data) -> client* {
      return static_cast<canvas*>(data)->client_.get();
    };

    emscripten_set_resize_callback(nullptr, this, EM_FALSE, [](int type, const EmscriptenUiEvent* e, void* data) {
      const auto cx = e->windowInnerWidth;
      const auto cy = e->windowInnerHeight;
      emscripten_set_canvas_size(cx, cy);
      get(data)->resize(static_cast<GLsizei>(cx), static_cast<GLsizei>(cy));
      return EM_TRUE;
    });

    emscripten_set_keypress_callback(nullptr, this, EM_FALSE, [](int type, const EmscriptenKeyboardEvent* e, void* data) {
      std::string_view key(e->key);
      if (!key.empty()) {
        return get(data)->on_key(e->key) ? EM_TRUE : EM_FALSE;
      }
      return EM_FALSE;
    });

    emscripten_set_keydown_callback(nullptr, this, EM_FALSE, [](int type, const EmscriptenKeyboardEvent* e, void* data) {
      const auto key = js::parse_key(e->code);
      if (key != js::key::none) {
        return get(data)->on_key(key, true, e->repeat ? true : false) ? EM_TRUE : EM_FALSE;
      }
      return EM_FALSE;
    });

    emscripten_set_keyup_callback(nullptr, this, EM_FALSE, [](int type, const EmscriptenKeyboardEvent* e, void* data) {
      const auto key = js::parse_key(e->code);
      if (key != js::key::none) {
        return get(data)->on_key(key, false, e->repeat ? true : false) ? EM_TRUE : EM_FALSE;
      }
      return EM_FALSE;
    });

    emscripten_set_mousedown_callback(id, this, EM_FALSE, [](int type, const EmscriptenMouseEvent* e, void* data) {
      const auto x = static_cast<double>(e->clientX);
      const auto y = static_cast<double>(e->clientY);
      return get(data)->on_button(static_cast<int>(e->button), x, y, true) ? EM_TRUE : EM_FALSE;
    });

    emscripten_set_mouseup_callback(id, this, EM_FALSE, [](int type, const EmscriptenMouseEvent* e, void* data) {
      const auto x = static_cast<double>(e->clientX);
      const auto y = static_cast<double>(e->clientY);
      return get(data)->on_button(static_cast<int>(e->button), x, y, false) ? EM_TRUE : EM_FALSE;
    });

    emscripten_set_wheel_callback(id, this, EM_FALSE, [](int type, const EmscriptenWheelEvent* e, void* data) {
      auto factor = 1.0;
      switch (e->deltaMode) {
      case DOM_DELTA_LINE: factor = scroll_line_factor; break;
      case DOM_DELTA_PAGE: factor = scroll_page_factor; break;
      }
      const auto cx = e->deltaX * factor;
      const auto cy = e->deltaY * factor;
      return get(data)->on_scroll(cx, cy) ? EM_TRUE : EM_FALSE;
    });

    emscripten_set_mousemove_callback(id, this, EM_FALSE, [](int type, const EmscriptenMouseEvent* e, void* data) {
      const auto x = static_cast<double>(e->clientX);
      const auto y = static_cast<double>(e->clientY);
      return get(data)->on_mouse(x, y) ? EM_TRUE : EM_FALSE;
    });

    emscripten_set_mouseenter_callback(id, this, EM_FALSE, [](int type, const EmscriptenMouseEvent* e, void* data) {
      return get(data)->on_mouse(true) ? EM_TRUE : EM_FALSE;
    });

    emscripten_set_mouseleave_callback(id, this, EM_FALSE, [](int type, const EmscriptenMouseEvent* e, void* data) {
      return get(data)->on_mouse(false) ? EM_TRUE : EM_FALSE;
    });

    show_canvas();
#else
    static auto get = [](GLFWwindow* window) -> client* {
      return static_cast<canvas*>(glfwGetWindowUserPointer(window))->client_.get();
    };

    glfwSetWindowSizeCallback(handle_, [](GLFWwindow* window, int cx, int cy) {
      get(window)->resize(static_cast<GLsizei>(cx), static_cast<GLsizei>(cy));
    });

    glfwSetCharCallback(handle_, [](GLFWwindow* window, unsigned int cp) {
      char beg[4];
      auto end = utf8::utf32to8(&cp, &cp + 1, beg);
      get(window)->on_key({ beg, static_cast<std::size_t>(end - beg) });
    });

    glfwSetKeyCallback(handle_, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
      get(window)->on_key(static_cast<js::key>(key), action ? true : false, action == 2);
    });

    glfwSetMouseButtonCallback(handle_, [](GLFWwindow* window, int button, int action, int mods) {
      double x = 0.0;
      double y = 0.0;
      glfwGetCursorPos(window, &x, &y);
      get(window)->on_button(button, x, y, action ? true : false);
    });

    glfwSetScrollCallback(handle_, [](GLFWwindow* window, double cx, double cy) {
      get(window)->on_scroll(-cx * scroll_line_factor, -cy * scroll_line_factor);
    });

    glfwSetCursorPosCallback(handle_, [](GLFWwindow* window, double x, double y) {
      get(window)->on_mouse(x, y);
    });

    glfwSetCursorEnterCallback(handle_, [](GLFWwindow* window, int entered) {
      get(window)->on_mouse(entered ? true : false);
    });
#endif
    client_->resize(static_cast<GLsizei>(cx), static_cast<GLsizei>(cy));
  }

  bool render() {
    client_->render();
#ifdef __EMSCRIPTEN__
    return true;
#else
    glfwSwapBuffers(handle_);
    glfwPollEvents();
#ifdef _DEBUG
    if (glfwGetKey(handle_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      return false;
    }
#endif
    return !glfwWindowShouldClose(handle_);
#endif
  }

  auto handle() const noexcept {
    return handle_;
  }

private:
#ifdef __EMSCRIPTEN__
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE handle_ = 0;
#else
  GLFWwindow* handle_ = nullptr;
#endif
  std::unique_ptr<client> client_;
};

#ifdef __EMSCRIPTEN__

int main(int argc, char *argv[]) {
  try {
    static canvas canvas;
    canvas.create(argc > 1 ? argv[1] : nullptr);
    emscripten_set_main_loop([]() {
      try {
        canvas.render();
      }
      catch (const std::exception& e) {
        emscripten_cancel_main_loop();
        js::error() << e.what();
      }
    }, 0, 0);
  }
  catch (const std::exception& e) {
    js::error() << e.what();
  }
}

#else

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR cmd, int show) {
#ifdef NDEBUG
  try {
#endif
    canvas canvas;
    canvas.create();
    while (canvas.render()) {
    }
#ifdef NDEBUG
  }
  catch (const std::exception& e) {
    std::wstring msg;
    msg.resize(MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, nullptr, 0) + 1);
    msg.resize(MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, &msg[0], static_cast<int>(msg.size())));
    MessageBox(nullptr, msg.data(), L"DEUS", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
    return 1;
  }
#endif
  return 0;
}

#endif
