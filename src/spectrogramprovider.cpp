#include "spectrogramprovider.h"
#include "audioFilesModel.h"
#include "datarasterfactory.h"
#include "fftforeman.h"
#include "spectrogramdatarasterinstance.h"
#include <QLinearGradient>
#include <QPainter>
#include <memory>
#include <qwt_color_map.h>
SpectrogramProvider::SpectrogramProvider(QQuickItem *parent)
    : QQuickPaintedItem(parent) {
  setMipmap(true);
  m_spectrogram->setRenderThreadCount(10);

  QwtLinearColorMap *colorMap =
      new QwtLinearColorMap(Qt::blue, Qt::red, QwtColorMap::Indexed);
  colorMap->addColorStop(0.2, Qt::cyan);
  colorMap->addColorStop(0.4, Qt::yellow);
  colorMap->addColorStop(0.8, Qt::magenta);
  m_spectrogram->setColorMap(colorMap);
  m_spectrogram->setColorTableSize(512);
}

void SpectrogramProvider::loadNewSpectrogramData(int audioFilesModelIndex) {
  std::cerr << "loadingNewSpectrogramData from index " << audioFilesModelIndex
            << "\n";
  m_current_file = m_audioFilesModel->getAudioFile(audioFilesModelIndex);
  std::cerr << "loaded in file from m_audioFilesModel\n";
  // implement way to get WavData from WavFile (just get STFT data)
  // TODO make STFT_DATA_MAGNITUDE for double compatibility
  fftforemanutils::STFT_DATA<double> STFT_Data = FFTForeman::STFT<double>(
      *m_current_file, CONFIG_window_length, CONFIG_hop_size);

  std::cerr << "made STFT data \n";
  auto dataRaster = DataRasterFactory::get_raster_data_instance<
      SpectrogramDataRasterInstance<double>, double,
      fftforemanutils::STFT_DATA<double>>(std::move(STFT_Data));
  std::cerr << "made dataRaster instance\n";

  m_spectrogram->setData(dataRaster.release());
  emit currentFileNameChanged();
  std::cerr << "set Data\n";
  emit currentFileDeltaTPerSampleChanged(m_current_file->getDeltaTPerSample());
  update();
}

void SpectrogramProvider::recomputeSpectrogram() {
  std::cerr << "Recompyting Spectrogram for window len " << CONFIG_window_length
            << " and hop size " << CONFIG_hop_size << "\n";
  fftforemanutils::STFT_DATA<double> STFT_Data = FFTForeman::STFT<double>(
      *m_current_file, CONFIG_window_length, CONFIG_hop_size);

  std::cerr << "made STFT data \n";
  auto dataRaster = DataRasterFactory::get_raster_data_instance<
      SpectrogramDataRasterInstance<double>, double,
      fftforemanutils::STFT_DATA<double>>(std::move(STFT_Data));
  std::cerr << "made dataRaster instance\n";

  m_spectrogram->setData(dataRaster.release());
  std::cerr << "set Data\n";
  update();
  std::cerr << "update call\n";
}

void SpectrogramProvider::paint(QPainter *painter) {
  if (!m_spectrogram->data()) {
    return; // Nothing to draw yet
  }
  QRectF rect = contentsBoundingRect();
  rect.translate(-CONFIG_x0, CONFIG_y0);

  QwtScaleMap xMap;
  xMap.setPaintInterval(rect.left(), rect.right());
  xMap.setScaleInterval(m_spectrogram->data()->interval(Qt::XAxis).minValue(),
                        m_spectrogram->data()->interval(Qt::XAxis).maxValue());

  QwtScaleMap yMap;
  yMap.setPaintInterval(rect.bottom(), rect.top());
  yMap.setScaleInterval(m_spectrogram->data()->interval(Qt::YAxis).minValue(),
                        m_spectrogram->data()->interval(Qt::YAxis).maxValue());

  painter->scale(CONFIG_xscale, CONFIG_yscale);
  painter->setClipRect(rect);
  m_spectrogram->draw(painter, xMap, yMap, rect);
}
// --- SpectrogramController ---
