#pragma once
#include "fftforeman.h"
#include <QwtInterval>
#include <QwtRasterData>
#include <algorithm>
template <typename T>
class SpectrogramDataRasterInstance : public QwtRasterData {
public:
  SpectrogramDataRasterInstance(fftforemanutils::STFT_DATA<T> data)
      : m_data(data) {

    std::cerr << "Got into SpecDataRasterInst constrcutor \n";
    m_timeInterval = QwtInterval(0, m_data.columns());
    m_freqInterval = QwtInterval(1, m_data.rows() / 2);
    std::cerr << "got intervals\n";
    m_magData = m_data.getData();
    std::cerr << "got magData\n";
    auto max = std::max_element(m_magData.begin(), m_magData.end());
    assert(max != m_magData.end() && "Error finding max element");
    std::cerr << "found max element of " << *max << "\n";

    m_magInterval = QwtInterval(100, *max / 1000);

    std::cerr << "Got m_magInterval\n";
  }

  double value(double x, double y) const override {

    int col = static_cast<int>(x);
    int row = static_cast<int>(y);

    size_t index = (col * m_data.rows() + row);
    if (index < m_magData.size()) {
      // std::cerr << "value: for (" << col << "," << row
      //<< "): " << m_magData[index] << "\n";
      return static_cast<double>(m_magData[index]);
    }
    return 0;
  }

  QwtInterval interval(Qt::Axis ax) const override {
    switch (ax) {
    case Qt::Axis::XAxis:
      return m_timeInterval;
    case Qt::Axis::YAxis:
      return m_freqInterval;
    case Qt::Axis::ZAxis:
      return m_magInterval;
    }
  }

private:
  fftforemanutils::STFT_DATA<T> m_data;
  std::vector<double> m_magData;
  QwtInterval m_timeInterval;
  QwtInterval m_freqInterval;
  QwtInterval m_magInterval;
};
