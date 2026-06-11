#pragma once
#include "Irastercompatibledata.h"
#include "sharedconstants.h"
#include "wavfile.h"
#include <algorithm>
#include <cmath>
#include <complex>
#include <cstdint>
#include <dependencies/dj_fft/dj_fft.h>
#include <math.h>
#include <memory>
#include <numbers>

namespace fftforemanutils {

template <typename T>
std::vector<std::complex<T>>
getComplexValuedBuffer(WavFile<SharedTypeDefs::WAVFILE_SAMPLE> wavFile) {
  int channel = 0;
  int numSamples = wavFile.m_AudioFile.getNumSamplesPerChannel();
  std::vector<std::complex<T>> outBuffer;
  for (int i = 0; i < numSamples; i++) {
    T currentSample = wavFile.m_AudioFile.samples[channel][i];
    outBuffer.push_back(std::complex<T>(currentSample, 0));
  }
  return outBuffer;
}

std::vector<double> get_frequency_bins_FFT(int N, int sampleRate) {
  std::vector<double> output;
  output.resize(N, double());

  for (int i = 0; i < N; i++) {
    output[i] = double(i);
  }
  return output;
}

using FFT_METADATA = struct {
  std::vector<double> frequencyBins;
  int sample_rate;
};

using STFT_METADATA = struct {
  int window_length;
  int hop_size;
  std::vector<double> frequencyBins;
  double delta_t_per_step;
  int num_steps;
};

template <typename T>
std::unique_ptr<std::vector<double>>
STFT_MAGNITUDE_DATA(const std::vector<std::complex<T>> &data) {
  auto output = std::make_unique<std::vector<double>>();
  output->resize(data.size(), double());
  for (int i = 0; i < data.size(); i++) {
    output->at(i) = norm(data[i]);
  }
  return std::move(output);
}
template <typename T> class FFT_DATA {
public:
  FFT_DATA(std::vector<std::complex<T>> data, FFT_METADATA metaData)
      : m_data(data), m_metadata(metaData) {}

  FFT_METADATA m_metadata;
  std::vector<std::complex<T>> m_data;
};

inline bool is_power_of_2(int value) {
  return std::floor(std::log2(value)) == std::log2(value);
}

template <typename T> class STFT_DATA : public IRasterCompatibleData<T> {
public:
  STFT_DATA(std::vector<std::complex<T>> data, STFT_METADATA metaData)
      : m_data(data), m_metadata(metaData) {
    m_magData = std::vector<double>();
    m_magData.resize(data.size(), double());
    for (int i = 0; i < data.size(); i++) {
      m_magData[i] = norm(data[i]);
    }
  }

  const std::vector<double> &getData() const override { return m_magData; }

  int rows() const override { return m_metadata.frequencyBins.size(); }

  int columns() const override { return m_metadata.num_steps; }

  std::vector<std::complex<T>> getComplexData() const { return m_data; }

private:
  std::vector<std::complex<T>> m_data;
  std::vector<double> m_magData;
  STFT_METADATA m_metadata;
};

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

    // std::cerr << "bitRep: " << bitRep << "\n";
    output[bitRep] = buff[k];
  }
  return output;
}
template <typename T>
inline void scale(std::vector<std::complex<T>> &v, const int n) {

  double sF = (1.0 / std::sqrt(n));
  for (auto &val : v) {
    val = val * (sF);
  }
}

template <typename T> void applyHanning(std::vector<std::complex<T>> &data) {
  int L = data.size();

  for (int n = 0; n < L; n++) {

    data[n] *=
        //(20.0 / L) *
        (.5 - .5 * std::cos((2 * std::numbers::pi * n) / L));
    // std::cout << "data[n]: " << data[n] << "\n";
  }
}

// template <typename T>
// void applyKaiser(double beta, std::vector<std::complex<T>> &data) {
//   int L = data.size();
//
//   for (int n = 0; n < L; n++) {
//     data[n] *= std::cyl_bessel_i(0, std::static_cast<double>(n));
//   }
// }
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
fftforemanutils::FFT_DATA<T> performFFT(std::vector<std::complex<T>> &data,
                                        int sample_rate) {
  ensure_power_of_2(data);
  std::vector<std::complex<T>> transformData =
      dj::fft1d<T>(data, dj::fft_dir::DIR_FWD);
  int n = size(transformData);
  fftforemanutils::FFT_METADATA metadata;
  metadata.frequencyBins = fftforemanutils::get_frequency_bins_FFT(
      transformData.size(), sample_rate);
  metadata.sample_rate = sample_rate;

  return fftforemanutils::FFT_DATA<T>(transformData, metadata);
}

template <wavfileUtils::Numeric T>
fftforemanutils::FFT_DATA<T> FFT(const WavFile<T> &wavFile) {
  std::vector<std::complex<T>> data =
      fftforemanutils::getComplexValuedBuffer(wavFile);

  return performFFT(data, wavFile.getSampleRate());
}

template <typename T>
fftforemanutils::STFT_DATA<T>
performSTFT(const std::vector<std::complex<T>> &data, int window_length,
            int hop_size, int sample_rate, double file_time) {

  size_t n = data.size();
  assert(fftforemanutils::is_power_of_2(window_length) &&
         "Window length is now power of 2");
  // std::cerr << "Window_length: " << window_length << "\n Hop_size: " <<
  // hop_size
  //         << "\n";
  assert(hop_size < window_length &&
         "Hop length must be smaller than window size");
  std::vector<std::complex<T>> outData;

  int numFrames =
      static_cast<int>(std::floor((n - window_length) / hop_size)) + 1;
  double delta_t_per_step = file_time / numFrames;
  int numBins = 0;

  fftforemanutils::STFT_METADATA metadata;

  for (int i = 0; i < numFrames; i++) {
    int win_start = i * hop_size;
    int win_end = win_start + window_length;

    std::vector<std::complex<T>> subset(data.begin() + win_start,
                                        data.begin() + win_end);
    fftforemanutils::applyHanning(subset);
    fftforemanutils::FFT_DATA<T> d = performFFT(subset, sample_rate);
    if (i == 0) {
      metadata.frequencyBins = d.m_metadata.frequencyBins;
    }
    // typename std::vector<std::complex<T>>::const_iterator first =
    //     d.m_data.begin();
    //
    // typename std::vector<std::complex<T>>::const_iterator second =
    //     d.m_data.begin() + std::ceil(d.m_data.size() / 2);
    //
    // d.m_data.assign(first, second);
    int numBins = d.m_data.size();

    outData.insert(outData.end(), d.m_data.begin(), d.m_data.end());
  }
  metadata.hop_size = hop_size;
  metadata.num_steps = numFrames;
  metadata.delta_t_per_step =
      (file_time * sample_rate) / window_length - hop_size;
  return fftforemanutils::STFT_DATA<T>(outData, metadata);
}

template <typename T>
fftforemanutils::STFT_DATA<T>
STFT(const WavFile<SharedTypeDefs::WAVFILE_SAMPLE> &file,
     int window_length = 64, int hop_size = 32) {
  std::cout << "inside of STFT\n";
  std::vector<std::complex<T>> data =
      fftforemanutils::getComplexValuedBuffer<T>(file);

  fftforemanutils::STFT_DATA<T> outputData =
      performSTFT<T>(data, window_length, hop_size, file.getSampleRate(),
                     file.GetLengthInSeconds());
  return outputData;
}
} // namespace FFTForeman
