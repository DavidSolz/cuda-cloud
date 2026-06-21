#pragma once

#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include <resource.hpp>

template <typename TBase> class Register {
private:
  using Traits      = ResourceTrait<TBase>;
  using NativeHandle = typename Traits::NativeHandle;
  using Descriptor   = typename Traits::Descriptor;
  using Allocator    = typename Traits::Allocator;

  std::vector<NativeHandle> _handles;
  std::vector<Descriptor>   _descriptors;
  std::vector<uint8_t>      _inUse;

  std::unordered_map<Descriptor, std::vector<uint32_t>> _mapping;
  std::vector<int32_t> _freeIndices;

public:
  TBase create(const Descriptor &descriptor) {
    uint32_t index;
    NativeHandle nativeHandle = Allocator::create(descriptor);

    if (!_freeIndices.empty()) {
      index = static_cast<uint32_t>(_freeIndices.back());
      _freeIndices.pop_back();

      const Descriptor &oldDescriptor = _descriptors[index];

      auto &v = _mapping[oldDescriptor];

      if (oldDescriptor != descriptor) {
        auto it = _mapping.find(oldDescriptor);
        if (it != _mapping.end()) {
          auto &v2 = it->second;
          v2.erase(std::remove(v2.begin(), v2.end(), index), v2.end());
        }
        _mapping[descriptor].push_back(index);
      }

      _handles[index]     = nativeHandle;
      _descriptors[index] = descriptor;
      _inUse[index]       = true;
    } else {
      index = static_cast<uint32_t>(_handles.size());
      _handles.push_back(nativeHandle);
      _descriptors.push_back(descriptor);
      _inUse.push_back(true);
      _mapping[descriptor].push_back(index);
    }

    TBase out(descriptor, _handles[index]);
    out.index = index;

    return out;
}

  TBase createOrGet(const Descriptor &descriptor) {
    auto it = _mapping.find(descriptor);

    if (it == _mapping.end() || it->second.empty()) {
      return create(descriptor);
    }

    auto freePtr = std::find_if(
        it->second.begin(), it->second.end(),
        [&](uint32_t index) {
          return !_inUse[index] || !Traits::isValid(_handles[index]);
        });

    if (freePtr != it->second.end()) {
      uint32_t index = *freePtr;

      if (!Traits::isValid(_handles[index])) {
        _handles[index]     = Allocator::create(descriptor);
        _descriptors[index] = descriptor;
      }

      _inUse[index] = true;

      TBase out(descriptor, _handles[index]);
      out.index = index;
      return out;
    }

    return create(descriptor);
  }

  void release(const TBase &base) { _inUse[base.index] = false; }

  void dispose(const TBase &base) {
    NativeHandle nativeHandle = _handles[base.index];

    _handles[base.index]     = NativeHandle{};
    _inUse[base.index]       = false;

    Allocator::dispose(nativeHandle);
    _freeIndices.push_back(base.index);
  }

  void clear() {
    for (size_t i = 0; i < _handles.size(); i++) {
      if (Traits::isValid(_handles[i])) {
        Allocator::dispose(_handles[i]);
      }
    }
    _handles.clear();
    _descriptors.clear();
    _inUse.clear();
    _mapping.clear();
    _freeIndices.clear();
  }

  std::vector<NativeHandle>& getHandles() const { return _handles; }

  ~Register() {
    for (size_t i = 0; i < _handles.size(); i++) {
      if (Traits::isValid(_handles[i])) {
        Allocator::dispose(_handles[i]);
      }
    }
  }
};