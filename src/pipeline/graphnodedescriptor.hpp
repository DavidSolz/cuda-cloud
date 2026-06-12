#pragma once

#include <memory>
#include <string>
#include <vector>

#include <nodeexecutor.hpp>

class GraphNodeDescriptor {
private:
  std::string _name;
  std::vector<std::string> _inputs;
  std::vector<std::string> _outputs;
  std::unique_ptr<NodeExecutor> _executor;

public:
  GraphNodeDescriptor() = default;

  GraphNodeDescriptor(const std::string &name,
                      std::unique_ptr<NodeExecutor> executor);

  GraphNodeDescriptor(const GraphNodeDescriptor &) = delete;
  GraphNodeDescriptor &operator=(const GraphNodeDescriptor &) = delete;

  GraphNodeDescriptor(GraphNodeDescriptor &&) noexcept = default;
  GraphNodeDescriptor &operator=(GraphNodeDescriptor &&) noexcept = default;

  std::string getName() const;

  GraphNodeDescriptor &addInput(const std::string &inputName);
  GraphNodeDescriptor &addOutput(const std::string &outputName);

  const std::vector<std::string> &getInputs() const;
  const std::vector<std::string> &getOutputs() const;

  NodeExecutor* getExecutor() const;

  ~GraphNodeDescriptor() = default;
};