#include <graphbuilder.hpp>

void GraphBuilder::createBindingIndexMap(
    std::unordered_map<std::string, int> &bindingToIndexMap) const {

  for (const auto &desc : _descriptors) {

    for (const auto &output : desc.getOutputs()) {

      if (bindingToIndexMap.find(output) != bindingToIndexMap.end()) {
        continue;
      }

      bindingToIndexMap[output] = bindingToIndexMap.size();
    }
  }
}

void GraphBuilder::computeNodeDegrees(std::vector<size_t> &incoming,
                                      std::vector<size_t> &outgoing) const {
  incoming.resize(_descriptors.size());
  outgoing.resize(_descriptors.size());

  for (size_t i = 0; i < _descriptors.size(); i++) {
    const auto &desc = _descriptors[i];
    incoming[i] = desc.getInputs().size();
    outgoing[i] = desc.getOutputs().size();
  }
}

void GraphBuilder::computeResourceNodeAdjacency(
    std::unordered_map<std::string, int> &bindingToIndexMap,
    std::vector<std::vector<int>> &adjacencyList) const {
  for (size_t i = 0; i < _descriptors.size(); i++) {
    const auto &desc = _descriptors[i];

    for (const auto &input : desc.getInputs()) {
      int inputIndex = bindingToIndexMap[input];
      adjacencyList[inputIndex].push_back(i);
    }
  }
}
void GraphBuilder::topologicalSort(
    std::unordered_map<std::string, int> &bindingToIndexMap,
    std::vector<std::vector<int>> &resourceNodeAdjacency,
    std::vector<size_t> &incoming, std::vector<int> &nodeExecutionOrder,
    std::vector<std::vector<int>> &children) const {

  std::vector<size_t> queue;

  for (size_t i = 0; i < incoming.size(); i++) {
    if (incoming[i] == 0) {
      queue.push_back(i);
    }
  }

  while (!queue.empty()) {
    int nodeIndex = queue.back();
    queue.pop_back();
    nodeExecutionOrder.push_back(nodeIndex);

    for (std::string bindingName : _descriptors[nodeIndex].getOutputs()) {
      int bindingIndex = bindingToIndexMap[bindingName];
      for (int adjacentNode : resourceNodeAdjacency[bindingIndex]) {
        incoming[adjacentNode]--;
        if (incoming[adjacentNode] == 0) {
          children[nodeIndex].push_back(adjacentNode);
          queue.push_back(adjacentNode);
        }
      }
    }
  }

  if (nodeExecutionOrder.size() != _descriptors.size()) {
    throw std::runtime_error("Graph has a cycle or disconnected components");
  }
}
Graph GraphBuilder::compile() const {

  std::unordered_map<std::string, int> bindingToIndexMap;
  createBindingIndexMap(bindingToIndexMap);

  for (const auto &desc : _descriptors) {
    for (const auto &input : desc.getInputs()) {

      if (bindingToIndexMap.find(input) == bindingToIndexMap.end()) {
        throw std::runtime_error("Input '" + input + "' of node '" +
                                 desc.getName() +
                                 "' is not produced by any node");
      }
    }
  }

  const size_t numBindings = bindingToIndexMap.size();

  std::vector<std::string> flattenedBindingNames(numBindings);
  for (const auto &pair : bindingToIndexMap) {
    flattenedBindingNames[pair.second] = pair.first;
  }

  std::vector<std::vector<int>> resourceNodeAdjacency(numBindings);
  computeResourceNodeAdjacency(bindingToIndexMap, resourceNodeAdjacency);

  std::vector<size_t> incoming;
  std::vector<size_t> outgoing;
  computeNodeDegrees(incoming, outgoing);

  std::vector<int> nodeExecutionOrder;
  std::vector<std::vector<int>> children(_descriptors.size());

  topologicalSort(bindingToIndexMap, resourceNodeAdjacency, incoming,
                  nodeExecutionOrder, children);

  std::vector<std::string> nodeNames(_descriptors.size());
  std::vector<NodeExecutor *> executors(_descriptors.size());

  for (size_t i = 0; i < nodeExecutionOrder.size(); i++) {
    auto nodeIndex = nodeExecutionOrder[i];
    nodeNames[i] = _descriptors[nodeIndex].getName();
    executors[i] = _descriptors[nodeIndex].getExecutor();
  }

  GraphStructuralDescriptor descriptor{nodeNames, executors};

  return Graph(descriptor);
}