#pragma once
#include "fftforeman.h"
#include <algorithm>
#include <qwt_interval.h>
#include <qwt_raster_data.h>
#include <qwt_transform.h>
template <typename T>
class SpectrogramDataRasterInstance : public QwtRasterData {
public:
  SpectrogramDataRasterInstance(fftforemanutils::STFT_DATA<T> data)
      : m_data(data), m_logTransform(QwtLogTransform()) {

    m_timeInterval = QwtInterval(0, m_data.columns());
    m_freqInterval = QwtInterval(1, m_data.rows() / 2);
    m_magData = m_data.getData();
    auto max = std::max_element(m_magData.begin(), m_magData.end());
    assert(max != m_magData.end() && "Error finding max element");

    m_magInterval = QwtInterval(m_logTransform.transform(100),
                                m_logTransform.transform(*max));
  }

  double value(double x, double y) const override {

    int col = static_cast<int>(x);
    int row = static_cast<int>(y);
    // if (x < 20) {
    //
    //   std::cerr << "Accessing col: " << col << "\n";
    // }

    size_t index = (col * m_data.rows() + row);
    if (index < m_magData.size()) {
      // std::cerr << "value: for (" << col << "," << row
      //<< "): " << m_magData[index] << "\n";
      return m_logTransform.transform(static_cast<double>(m_magData[index]));
    }
    return -100;
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
  QwtLogTransform m_logTransform;
};
