#pragma once

struct Resource {
  int index;
  virtual ~Resource() = default;
};

template<typename TBase>
struct ResourceTrait {};

template<typename TBase>
struct Allocator {};