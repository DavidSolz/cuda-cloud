#pragma once

#include <nodeexecutor.hpp>
#include <string>
#include <vector>

struct GraphStructuralDescriptor {
  std::vector<std::string> nodeNames;
  std::vector<NodeExecutor*> executors;
};

class Graph {
private:
  GraphStructuralDescriptor _descriptor;

public:
  explicit Graph(GraphStructuralDescriptor descriptor)
      : _descriptor(descriptor) {};

  ~Graph() = default;
};