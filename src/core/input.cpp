#include <input.hpp>

Input::Input(NativeWindowHandle window) : _window(window) {

  _deviceRanges[static_cast<uint32_t>(DeviceType::Keyboard)] = {
      0, GLFW_KEY_LAST + 1};
  _deviceRanges[static_cast<uint32_t>(DeviceType::Mouse)] = {
      GLFW_KEY_LAST + 1, GLFW_MOUSE_BUTTON_LAST + 1};

  _current.resize(GLFW_KEY_LAST + 1 + GLFW_MOUSE_BUTTON_LAST + 1, 0);
  _previous.resize(GLFW_KEY_LAST + 1 + GLFW_MOUSE_BUTTON_LAST + 1, 0);
}

uint32_t Input::getIndex(DeviceType device, BindingIndex index) const {
  size_t deviceIndex = static_cast<size_t>(device);
  if (deviceIndex >= _deviceRanges.size()) {
    throw std::runtime_error("Invalid device type");
  }

  auto range = _deviceRanges[deviceIndex];

  return range.offset + index;
}

void Input::bindAction(const std::string &name, DeviceType device, int code) {

  uint32_t index = getIndex(device, code);

  for (const auto &existingBinding : _bindings[name]) {
    if (existingBinding == code) {
      return;
    }
  }

  _bindings[name].push_back(code);
}

bool Input::getMouse(const std::string &name) const {
  const auto &bindings = _bindings.find(name);
  if (bindings == _bindings.end()) {
    return false;
  }

  for (const auto &index : bindings->second) {
    if (_current[index])
      return true;
  }

  return false;
}

bool Input::getMouseDown(const std::string &name) const {

  const auto &bindings = _bindings.find(name);
  if (bindings == _bindings.end()) {
    return false;
  }

  for (const auto &index : bindings->second) {
    if (_current[index] && !_previous[index])
      return true;
  }

  return false;
}

bool Input::getMouseUp(const std::string &name) const {

  const auto &bindings = _bindings.find(name);

  if (bindings == _bindings.end()) {
    return false;
  }

  for (const auto &index : bindings->second) {
    if (!_current[index] && _previous[index])
      return true;
  }

  return false;
}

bool Input::getButton(const std::string &name) const {
  const auto &bindings = _bindings.find(name);
  if (bindings == _bindings.end()) {
    return false;
  }

  for (const auto &index : bindings->second) {
    if (_current[index])
      return true;
  }

  return false;
}

bool Input::getButtonDown(const std::string &name) const {
  const auto &bindings = _bindings.find(name);
  if (bindings == _bindings.end()) {
    return false;
  }

  for (const auto &index : bindings->second) {
    if (_current[index] && !_previous[index])
      return true;
  }

  return false;
}

bool Input::getButtonUp(const std::string &name) const {
  const auto &bindings = _bindings.find(name);
  if (bindings == _bindings.end()) {
    return false;
  }

  for (const auto &index : bindings->second) {
    if (!_current[index] && _previous[index])
      return true;
  }

  return false;
}

void Input::update() {

  _previous.swap(_current);

  for (int key = 0; key <= GLFW_KEY_LAST; key++) {
    uint32_t index = getIndex(DeviceType::Keyboard, key);
    _current[index] = glfwGetKey(_window, key) == GLFW_PRESS;
  }

  uint32_t mouseOffset = _deviceRanges[(int)DeviceType::Mouse].offset;

  for (int button = 0; button <= GLFW_MOUSE_BUTTON_LAST; button++) {
    uint32_t index = getIndex(DeviceType::Mouse, button);
    _current[index] = glfwGetMouseButton(_window, button) == GLFW_PRESS;
  }
}