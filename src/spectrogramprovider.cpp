#include "spectrogramprovider.h"
#include "audioFilesModel.h"
#include "datarasterfactory.h"
#include "fftforeman.h"
#include "sharedconstants.h"
#include "spectrogramdatarasterinstance.h"
#include <QLinearGradient>
#include <QPainter>
#include <memory>

SpectrogramProvider::SpectrogramProvider(QQuickItem *parent)
    : QQuickPaintedItem(parent) {}

void SpectrogramProvider::loadNewSpectrogramData(int audioFilesModelIndex) {
  std::cerr << "loadingNewSpectrogramData from index " << audioFilesModelIndex
            << "\n";
  std::unique_ptr<WavFile<SharedTypeDefs::WAVFILE_SAMPLE>> requestedFile =
      m_audioFilesModel->getAudioFile(audioFilesModelIndex);
  std::cerr << "loaded in file from m_audioFilesModel\n";
  // implement way to get WavData from WavFile (just get STFT data)
  // TODO make STFT_DATA_MAGNITUDE for double compatibility
  fftforemanutils::STFT_DATA<double> STFT_Data =
      FFTForeman::STFT<double>(*requestedFile.release(), 256, 128);
  std::cerr << "made STFT data \n";
  auto dataRaster = DataRasterFactory::get_raster_data_instance<
      SpectrogramDataRasterInstance<double>, double,
      fftforemanutils::STFT_DATA<double>>(std::move(STFT_Data));
  std::cerr << "made dataRaster instance\n";

  m_spectrogram->setData(dataRaster.release());
  std::cerr << "set Data\n";
  update();
}

void SpectrogramProvider::paint(QPainter *painter) {
  if (!m_spectrogram->data()) {
    return; // Nothing to draw yet
  }
  painter->scale(CONFIG_xscale, CONFIG_yscale);
  // 1. Get the current size of the QML Item (in pixels)
  const QRectF rect = contentsBoundingRect();

  // 2. Setup the X-Axis Map (Time)
  QwtScaleMap xMap;
  xMap.setPaintInterval(rect.left(), rect.right());
  xMap.setScaleInterval(
      CONFIG_xscale * m_spectrogram->data()->interval(Qt::XAxis).minValue(),
      CONFIG_xscale * m_spectrogram->data()->interval(Qt::XAxis).maxValue());

  // 3. Setup the Y-Axis Map (Frequency)
  // Note: In UI coordinates, 0 is at the top, so we often flip the paint
  // interval
  QwtScaleMap yMap;
  yMap.setPaintInterval(rect.bottom(), rect.top());
  yMap.setScaleInterval(
      CONFIG_yscale * m_spectrogram->data()->interval(Qt::YAxis).minValue(),
      CONFIG_yscale * m_spectrogram->data()->interval(Qt::YAxis).maxValue());

  // 4. Draw the spectrogram
  // The third argument (canvasRect) is used for clipping; usually the same as
  // rect.
  m_spectrogram->draw(painter, xMap, yMap, rect);
}
// --- SpectrogramController ---
