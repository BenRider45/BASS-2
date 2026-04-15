#include "wavfile.h"
#include <bit>
#include <bitset>
#include <complex>
#include <cstdint>
#include <math.h>
#include <numbers>

namespace fftforemanutils {
template <typename T>
std::vector<std::complex<T>> getComplexValuedBuffer(WavFile<T> wavFile) {
  int channel = 0;
  int numSamples = wavFile.m_AudioFile.getNumSamplesPerChannel();
  std::vector<std::complex<T>> outBuffer;
  for (int i = 0; i < numSamples; i++) {
    double currentSample = wavFile.m_AudioFile.samples[channel][i];
    outBuffer.push_back(std::complex<T>(currentSample, 0));
  }
  return outBuffer;
}

inline int bitr(uint32_t x, int nb) {
  assert(nb > 0 && 32 > nb && "invalid bit count");
  x = (x << 16) | (x >> 16);
  x = ((x & 0x00FF00FF) << 8) | ((x & 0xFF00FF00) >> 8);
  x = ((x & 0x0F0F0F0F) << 4) | ((x & 0xF0F0F0F0) >> 4);
  x = ((x & 0x33333333) << 2) | ((x & 0xCCCCCCCC) >> 2);
  x = ((x & 0x55555555) << 1) | ((x & 0xAAAAAAAA) >> 1);

  return ((x >> (32 - nb)) & (0xFFFFFFFF >> (32 - nb)));
}

template <typename T>
std::vector<T> get_bit_reversed_buffer(const std::vector<T> &buff) {
  std::cerr << "In get reversed Buffer \n";
  std::vector<T> output;
  output.resize(buff.size(), T());
  const unsigned int n = buff.size();

  for (int k = 0; k < n; k++) {
    uint32_t bitRep = fftforemanutils::bitr(k, std::ceil(std::log2(n)));

    std::cerr << "bitRep: " << bitRep << "\n";
    output[bitRep] = buff[k];
  }
  return output;
}

} // namespace fftforemanutils

namespace FFTForeman {

template <typename T> void ensure_power_of_2(std::vector<T> &data) {
  int len = data.size();
  if (std::floor(std::log2(len)) == std::log2(len)) {
    return;
  }
  int k = std::floor(std::log2(len));
  int n_delta = std::exp2(k + 1) - len;
  data.resize(len + n_delta, T());
  return;
}

template <typename T>
std::vector<std::complex<double>>
performFFT(std::vector<std::complex<T>> &data) {
  FFTForeman::ensure_power_of_2<std::complex<T>>(data);
  std::vector<std::complex<T>> revData =
      fftforemanutils::get_bit_reversed_buffer(data);

  int n = data.size();
  int m;
  std::complex<double> w_m;
  std::cerr << "Starting Transform\n";
  for (int s = 1; s <= std::log2(n); s++) {
    m = std::exp2(2);
    w_m = std::polar(1.0, -2 * std::numbers::pi / m);
    for (int k = 0; k <= n - 1; k += m) {
      std::complex<double> w = std::polar(double(0), 0.0);
      for (int j = 0; j < m / 2; j++) {
        auto t = w * revData[k + j + m / 2];
        auto u = revData[k + j];
        revData[k + j] = u + t;
        std::cerr << "revData[" << k << " + " << j << "] = " << revData[k + j];
        revData[k + j + m / 2] = u - t;

        std::cerr << "revData[" << k << " + " << j << "+" << m / 2
                  << "] = " << revData[k + j];
        w = w * w_m;
      }
    }
  }
  return fftforemanutils::get_bit_reversed_buffer(revData);
}

template <wavfileUtils::Numeric T>
std::vector<std::complex<double>> FFT(WavFile<T> wavFile) {
  std::vector<std::complex<double>> data =
      fftforemanutils::getComplexValuedBuffer(wavFile);

  return performFFT(data);
}
} // namespace FFTForeman
