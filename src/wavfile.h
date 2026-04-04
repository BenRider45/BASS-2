#include <string>
#include <type_traits>
#include <vector>

namespace wavfileUtils {
template <typename T>
concept Numeric = requires(T a) { std::is_arithmetic<T>(); };

template <Numeric T> using WavBuffer = std::vector<T>;

template <Numeric T> struct WavFileData {
  WavBuffer<T> WavData;
};

} // namespace wavfileUtils

class WavFile {
public:
  WavFile(std::string filePath);

private:
  wavfileUtils::WavBuffer<uint32_t> _dataBuffer;
};
