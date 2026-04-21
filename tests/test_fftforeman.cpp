#include "fftforeman.h"
#include "sharedconstants.h"
#include "wavsurfer.h"
#include <QDir>

#include <cmath>
#include <gtest/gtest.h>
#include <ranges>

#include <dependencies/dj_fft/dj_fft.h>
class test_FFTForeman : public testing::Test {

protected:
  void SetUp() override {
    QDir testPath(std::filesystem::current_path());

    std::cerr << "Current Path: " << testPath.absolutePath().toStdString()
              << "\n";

    testDataDir = testPath;
    testDataDir.cdUp();
    testDataDir.cdUp();
    assert(testDataDir.cd(QString("tests/test_data/fftforeman")));

    testWavDataPath = testDataDir.absolutePath();

    QStringList lst;
    lst << "*" + QString(".wav");
    testDataDir.setNameFilters(lst);
    testDataEntryList = testDataDir.entryList();

    for (const auto &entry : testDataEntryList) {
      std::cerr << entry.toStdString() << "\n";
    }
  }

  void TearDown() override {}

  QString folderName;
  QString testWavDataPath;
  QString projDirPath;
  QDir testDataDir;
  QStringList testDataEntryList;
};

TEST_F(test_FFTForeman, testPowerOfTwoFormatter) {

  std::vector<int> Startlengths = {7, 8, 9, 15, 17, 31, 34, 62};

  std::vector<int> Expectedlengths = {8, 8, 16, 16, 32, 32, 64, 64};

  for (auto &&[start, expected] :
       std::views::zip(Startlengths, Expectedlengths)) {
    std::vector<double> v;
    v.resize(start, double(1));
    std::vector<double> copyV(v);
    FFTForeman::ensure_power_of_2(v);

    ASSERT_EQ(expected, v.size());

    for (int i = 0; i < start; i++) {
      ASSERT_EQ(v[i], copyV[i]);
    }
    for (int i = start; i < expected; i++) {
      ASSERT_EQ(v[i], double());
    }
  }
}

TEST_F(test_FFTForeman, testBitReversalFuction) {
  std::vector<std::string> data = {"000", "001", "010", "011",
                                   "100", "101", "110", "111"};
  std::vector<std::string> BitRevData =
      fftforemanutils::get_bit_reversed_buffer(data);
  std::vector<std::string> ExpectedRevData = {"000", "100", "010", "110",

                                              "001", "101", "011", "111"};
  ASSERT_EQ(BitRevData, ExpectedRevData);
}

TEST_F(test_FFTForeman, testTransform) {

  WavSurfer<SharedTypeDefs::WAVFILE_SAMPLE> surfer;
  WavSurferUtils::WavFileList<SharedTypeDefs::WAVFILE_SAMPLE> files =
      surfer.getWavFilesFromNonProjectDir(testWavDataPath);
  std::vector<std::complex<double>> data = {
      std::complex<double>(1, 0), std::complex<double>(0, 0),
      std::complex<double>(0, 0), std::complex<double>(0, 0),
      std::complex<double>(0, 0), std::complex<double>(0, 0),
      std::complex<double>(0, 0), std::complex<double>(0, 0),
  };
  fftforemanutils::FFT_DATA<double> out = FFTForeman::performFFT(data, 1);
  std::vector<std::complex<double>> outRef =
      dj::fft1d(data, dj::fft_dir::DIR_FWD);
  ASSERT_EQ(out.m_data.size(), data.size());
  for (const auto &&[point, refPoint] : std::views::zip(out.m_data, outRef)) {
    std::cerr << "Mine: " << point << " Reference: " << refPoint << "\n";
  }

  // for (auto const &file : files) {
  //   std::vector<std::complex<double>> data =
  //       fftforemanutils::getComplexValuedBuffer<double>(file);
  //   fftforemanutils::FFT_DATA<double> dataOutMine =
  //       FFTForeman::performFFT<double>(data, 1);
  //   std::vector<std::complex<double>> dataOutRef =
  //       dj::fft1d(data, dj::fft_dir::DIR_FWD);
  //   for (auto const &&[point, refPoint] :
  //        std::views::zip(dataOutMine.m_data, dataOutRef)) {
  //     if (point.real() > 0 || point.imag() > 0) {
  //       std::cerr << "Found Bin!: " << point << "\t";
  //       std::cerr << "That Bin in Ref: " << refPoint << "\n";
  //     }
  //   }
  //   ASSERT_EQ(dataOutMine.m_data, dataOutRef);
  // }
  // ASSERT_EQ(out.m_data, outRef);
}

TEST_F(test_FFTForeman, OutputFrameCount) {
  int n = 256, window = 2, hop = 1;
  std::vector<std::complex<double>> signal(n, {1.0f, 0.0f});

  auto result = FFTForeman::performSTFT(signal, window, hop, 256, 2);

  int expected_frames = std::floor((n - window) / hop) + 1;
  EXPECT_EQ(result.columns(), expected_frames); // 7 frames
                                                //
                                                //
}

// TEST_F(test_FFTForeman, OutputFrameLength) {
//   std::vector<std::complex<float>> signal(256, {1.0f, 0.0f});
//   auto result = FFTForeman::performSTFT(signal, 64, 32);
//
//   for (const auto &frame : result)
//     EXPECT_EQ(frame.size(), 64);
// }
//
// TEST_F(test_FFTForeman, PureToneEnergyAtCorrectBin) {
//   int window = 64, hop = 32, n = 256;
//   int target_bin = 4; // cycles per window
//   std::vector<std::complex<float>> signal(n);
//
//   for (int i = 0; i < n; i++) {
//     float phase = 2.0f * M_PI * target_bin * i / window;
//     signal[i] = {std::cos(phase), std::sin(phase)};
//   }
//
//   auto result = FFTForeman::performSTFT(signal, window, hop);
//
//   for (const auto &frame : result) {
//     float peak = std::abs(frame[target_bin]);
//     ASSERT_GT(peak, 1e-6f)
//         << "Peak bin has negligible energy — wrong bin or signal gen bug";
//
//     for (int k = 0; k < (int)frame.size(); k++) {
//       if (k != target_bin) {
//         float ratio = std::abs(frame[k]) / peak;
//         EXPECT_LT(ratio, 0.01f) << "Bin " << k << " ratio: " << ratio;
//       }
//     }
//   }
// }
//
// TEST_F(test_FFTForeman, DCSignalEnergyAtBinZero) {
//   std::vector<std::complex<float>> signal(256, {1.0f, 0.0f});
//   auto result = FFTForeman::performSTFT(signal, 64, 32);
//
//   for (const auto &frame : result) {
//     EXPECT_NEAR(std::abs(frame[0]), std::sqrt(64.0),
//                 1e-3f); // DC bin = sum of inputs
//     for (int k = 1; k < (int)frame.size(); k++)
//       EXPECT_NEAR(std::abs(frame[k]), 0.0f, 1e-3f);
//   }
// }

// TEST_F(test_FFTForeman, SignalExactlyOneWindow) {
//   // Minimum valid input: signal length == window length → 1 frame
//   std::vector<std::complex<float>> signal(64, {1.0f, 0.0f});
//   auto result = FFTForeman::performSTFT(signal, 64, 32);
//   EXPECT_EQ(result.size(), 1);
// }
//
// TEST_F(test_FFTForeman, HopEqualsWindowNoOverlap) {
//   // hop == window → no overlap, frames tile exactly
//   int n = 256, window = 64;
//   std::vector<std::complex<float>> signal(n, {1.0f, 0.0f});
//   auto result = FFTForeman::performSTFT(signal, window, window);
//   EXPECT_EQ(result.size(), n / window);
// }

// TEST(test_FFTForeman, RoundTripReconstruction) {
//   std::vector<std::complex<float>> original = makeTestSignal(256);
//   auto stft_result = FFTForeman::performSTFT(original, 64, 32);
//   auto reconstructed = performISTFT(stft_result, 64, 32);
//
//   for (int i = 0; i < (int)original.size(); i++)
//     EXPECT_NEAR(std::abs(reconstructed[i] - original[i]), 0.0f, 1e-4f);
// }
