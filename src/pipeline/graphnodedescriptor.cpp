#include <graphnodedescriptor.hpp>

GraphNodeDescriptor::GraphNodeDescriptor(const std::string &name,
                                         std::unique_ptr<NodeExecutor> executor)
    : _name(name), _executor(std::move(executor)) {}

std::string GraphNodeDescriptor::getName() const { return _name; }

GraphNodeDescriptor &
GraphNodeDescriptor::addInput(const std::string &inputName) {

  auto it = std::find(_inputs.begin(), _inputs.end(), inputName);
  if (it != _inputs.end()) {
    throw std::runtime_error("Input with name '" + inputName +
                             "' already exists in node '" + _name + "'.");
  }

  _inputs.push_back(inputName);
  return *this;
}

GraphNodeDescriptor &
GraphNodeDescriptor::addOutput(const std::string &outputName) {

  auto it = std::find(_outputs.begin(), _outputs.end(), outputName);
  if (it != _outputs.end()) {
    throw std::runtime_error("Output with name '" + outputName +
                             "' already exists in node '" + _name + "'.");
  }

  _outputs.push_back(outputName);
  return *this;
}

const std::vector<std::string> &GraphNodeDescriptor::getInputs() const {
  return _inputs;
}

const std::vector<std::string> &GraphNodeDescriptor::getOutputs() const {
  return _outputs;
}

NodeExecutor* GraphNodeDescriptor::getExecutor() const {
  return _executor.get();
}