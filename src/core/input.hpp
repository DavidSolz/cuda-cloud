#pragma once

#include <GLFW/glfw3.h>
#include <array>
#include <stdexcept>
#include <string>
#include <unordered_map>


class Input {

public:
  using NativeWindowHandle = GLFWwindow *;

  enum class DeviceType { Keyboard = 0x01, Mouse = 0x02 };

  Input(NativeWindowHandle window);

  void bindAction(const std::string &name, DeviceType device, int code);

  void update();

  bool getMouse(const std::string &name) const;

  bool getMouseDown(const std::string &name) const;

  bool getMouseUp(const std::string &name) const;

  bool getButton(const std::string &name) const;

  bool getButtonDown(const std::string &name) const;

  bool getButtonUp(const std::string &name) const;

private:
  using BindingIndex = uint32_t;

  struct DeviceRange {
    uint32_t offset;
    uint32_t size;
  };

  std::array<DeviceRange, 256> _deviceRanges;

  NativeWindowHandle _window;

  std::unordered_map<std::string, std::vector<BindingIndex>> _bindings;

  std::vector<uint8_t> _current;
  std::vector<uint8_t> _previous;

  uint32_t getIndex(DeviceType device, BindingIndex index) const;
};
