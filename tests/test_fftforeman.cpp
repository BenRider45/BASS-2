#include "fftforeman.h"
#include "wavfile.h"
#include "wavsurfer.h"
#include <QDir>
#include <gtest/gtest.h>
#include <ranges>
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

  WavSurfer<double> surfer;
  WavSurferUtils::WavFileList<double> files =
      surfer.getWavFilesFromNonProjectDir(testWavDataPath);
  std::vector<std::complex<double>> data = {
      std::complex<double>(1, 0), std::complex<double>(0, 0),
      std::complex<double>(0, 0), std::complex<double>(0, 0)};
  std::vector<std::complex<double>> out = FFTForeman::performFFT(data);
  ASSERT_EQ(out.size(), data.size());
  for (const auto &point : out) {
    std::cerr << point << "\n";
  }
  ASSERT_FALSE(true);
}
