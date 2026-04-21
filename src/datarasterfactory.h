#pragma once
#include "Irastercompatibledata.h"
#include <concepts>
#include <iostream>
#include <qwt_raster_data.h>

template <typename T>
concept InheritsQwtRasterData = std::derived_from<T, QwtRasterData>;

template <typename T, typename V>
concept IsRasterableData = std::derived_from<T, IRasterCompatibleData<V>>;

class DataRasterFactory {

public:
  template <InheritsQwtRasterData T, typename W, IsRasterableData<W> V>
  static std::unique_ptr<T> get_raster_data_instance(const V &&data) {
    std::cerr << "Got into factory \n";
    return std::make_unique<T>(data);
  };
};
