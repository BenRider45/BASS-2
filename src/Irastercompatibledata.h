#pragma once
#include <vector>
template <typename T> class IRasterCompatibleData {
public:
  virtual ~IRasterCompatibleData() = default;

  virtual const std::vector<T> &getData() const = 0;

  virtual int columns() const = 0;
  virtual int rows() const = 0;
};
