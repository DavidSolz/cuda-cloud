#pragma once

#include <algorithm>
#include <graph.hpp>
#include <graphnodedescriptor.hpp>

#include <unordered_map>


class GraphBuilder {
private:
  std::vector<GraphNodeDescriptor> _descriptors;

  void createBindingIndexMap(
      std::unordered_map<std::string, int> &bindingToIndexMap) const;

  void computeNodeDegrees(std::vector<size_t> &incoming,
                          std::vector<size_t> &outgoing) const;

  void computeResourceNodeAdjacency(
      std::unordered_map<std::string, int> &bindingToIndexMap,
      std::vector<std::vector<int>> &adjacencyList) const;

  void topologicalSort(std::unordered_map<std::string, int> &bindingToIndexMap,
                       std::vector<std::vector<int>> &resourceNodeAdjacency,
                       std::vector<size_t> &incoming,
                       std::vector<int> &nodeExecutionOrder,
                       std::vector<std::vector<int>> &children) const;

public:
  GraphBuilder() = default;

  template <typename T, typename... Args>
  GraphNodeDescriptor &addNode(const std::string &name, Args &&...args) {
    static_assert(std::is_base_of_v<NodeExecutor, T>,
                  "T must derive from NodeExecutor");

    auto it = std::find_if(_descriptors.begin(), _descriptors.end(),
                           [&](const GraphNodeDescriptor &desc) {
                             return desc.getName() == name;
                           });

    if (it != _descriptors.end()) {
      throw std::runtime_error("Node with name '" + std::string(name) +
                               "' already exists");
    }

    std::unique_ptr<NodeExecutor> executor =
        std::make_unique<T>(std::forward<Args>(args)...);

    _descriptors.emplace_back(name, std::move(executor));

    return _descriptors.back();
  }

  Graph compile() const;
};