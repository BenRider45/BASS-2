#include "audioFilesModel.h"
#include <QString>
#include <QtQuick/QQuickPaintedItem>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_map.h>
class SpectrogramProvider : public QQuickPaintedItem {
  Q_OBJECT
  Q_PROPERTY(AudioFilesModel *audioFilesModel READ audioFilesModel WRITE
                 setAudioFilesModel NOTIFY audioFilesModelChanged)

  Q_PROPERTY(double CONFIG_xscale READ xscale WRITE set_xscale NOTIFY
                 RenderConfigChanged);
  Q_PROPERTY(double CONFIG_yscale READ yscale WRITE set_yscale NOTIFY
                 RenderConfigChanged);
  Q_PROPERTY(double CONFIG_color_scale READ color_scale WRITE set_color_scale
                 NOTIFY RenderConfigChanged);
  Q_PROPERTY(double CONFIG_x0 READ x0 WRITE set_x0 NOTIFY RenderConfigChanged);
  Q_PROPERTY(double CONFIG_y0 READ y0 WRITE set_y0 NOTIFY RenderConfigChanged);
  Q_PROPERTY(double CONFIG_hop_size READ hop_size WRITE set_hop_size NOTIFY
                 RenderConfigChanged);
  Q_PROPERTY(double CONFIG_window_length READ window_length WRITE
                 set_window_length NOTIFY RenderConfigChanged);
  Q_PROPERTY(
      double current_File_Delta_T_Per_Sample READ currentFileDeltaTPerSample
          NOTIFY currentFileDeltaTPerSampleChanged);
  Q_PROPERTY(QString currentFileName READ currentFileName NOTIFY
                 currentFileNameChanged);

public:
  enum CONFIG_TYPE { XSCALE, YSCALE, X0, Y0, HOP_SIZE, WINDOW_LENGTH };
  Q_ENUM(CONFIG_TYPE)

  explicit SpectrogramProvider(QQuickItem *parent = nullptr);

  void paint(QPainter *painter) override;

  double xscale() const { return CONFIG_xscale; };
  double yscale() const { return CONFIG_yscale; };
  double color_scale() const { return CONFIG_color_scale; };
  QString currentFileName() const {
    if (m_current_file) {
      return m_current_file->getFileName();
    }
    return QString();
  }
  int ScreenColumnToSampleIndex(double x);
  double x0() const { return CONFIG_x0; };
  double y0() const { return CONFIG_y0; };
  double hop_size() const { return CONFIG_hop_size; }
  double window_length() const { return CONFIG_window_length; }
  double currentFileDeltaTPerSample() const {
    return m_current_file->getDeltaTPerSample();
  }
  void set_xscale(double val) {
    CONFIG_xscale = val;
    emit RenderConfigChanged();
  };
  void set_yscale(double val) {
    CONFIG_yscale = val;
    emit RenderConfigChanged();
  };
  void set_color_scale(double val) {
    CONFIG_color_scale = val;
    emit RenderConfigChanged();
  };
  void set_x0(double val) {
    CONFIG_x0 = val;
    emit RenderConfigChanged();
  };
  void set_y0(double val) {
    CONFIG_y0 = val;
    emit RenderConfigChanged();
  };
  void set_hop_size(double val) {
    // std::cerr << "setting hop length\n";
    CONFIG_hop_size = val;
    emit RenderConfigChanged();
  }
  void set_window_length(double val) {
    //  std::cerr << "setting window length\n";
    CONFIG_window_length = val;
    emit RenderConfigChanged();
  }
  Q_INVOKABLE void loadNewSpectrogramData(int AudioFilesModelIndex);
  Q_INVOKABLE void crement_CONFIG_value(CONFIG_TYPE type, double crementValue,
                                        bool increment = true) {
    switch (type) {
    case CONFIG_TYPE::XSCALE:
      if (increment) {
        modify_CONFIG_value(xscale() + crementValue, type);
      } else {
        modify_CONFIG_value(xscale() - crementValue, type);
      }
      break;

    case CONFIG_TYPE::YSCALE:

      if (increment) {
        modify_CONFIG_value(yscale() + crementValue, type);
      } else {
        modify_CONFIG_value(yscale() - crementValue, type);
      }
      break;
    case CONFIG_TYPE::X0:

      if (increment) {
        modify_CONFIG_value(x0() + crementValue, type);
      } else {
        modify_CONFIG_value(x0() - crementValue, type);
      }
      break;
    case CONFIG_TYPE::Y0:

      if (increment) {
        modify_CONFIG_value(y0() + crementValue, type);
      } else {
        modify_CONFIG_value(y0() - crementValue, type);
      }
      break;

    case CONFIG_TYPE::HOP_SIZE:
      break;
    case CONFIG_TYPE::WINDOW_LENGTH:
      break;
    }
  }
  Q_INVOKABLE void modify_CONFIG_value(double value, CONFIG_TYPE type) {
    switch (type) {
    case CONFIG_TYPE::XSCALE:
      set_xscale(value);
      break;
    case CONFIG_TYPE::YSCALE:
      set_yscale(value);
      break;
    case CONFIG_TYPE::X0:
      set_x0(value);
      break;
    case CONFIG_TYPE::Y0:
      set_y0(value);
      break;
    case CONFIG_TYPE::HOP_SIZE:
      set_hop_size(value);
      // recomputeSpectrogram();
      break;
    case CONFIG_TYPE::WINDOW_LENGTH:
      //   std::cerr << "Got only call site for set_window_length\n";
      set_window_length(value);
      // recomputeSpectrogram();
      break;
    }

    update();
    return;
  }

  AudioFilesModel *audioFilesModel() const { return m_audioFilesModel; }
  void setAudioFilesModel(AudioFilesModel *model) {
    if (m_audioFilesModel == model)
      return;
    m_audioFilesModel = model;
    emit audioFilesModelChanged();
  }
  Q_INVOKABLE void recomputeSpectrogram();
signals:
  void renderingStarted();
  void renderingFinished();
  void dataLoaded();
  void audioFilesModelChanged();
  void RenderConfigChanged();
  void currentFileDeltaTPerSampleChanged(double value);
  void currentFileNameChanged();

private:
  QwtPlotSpectrogram *m_spectrogram = new QwtPlotSpectrogram();
  std::unique_ptr<WavFile<SharedTypeDefs::WAVFILE_SAMPLE>> m_current_file;
  AudioFilesModel *m_audioFilesModel;
  QwtScaleMap m_xMap;
  QwtScaleMap m_yMap;
  double CONFIG_xscale = 1;
  double CONFIG_yscale = 1;
  double CONFIG_hop_size = 8;
  double CONFIG_window_length = 256;
  double CONFIG_color_scale;
  double CONFIG_x0 = 0;
  double CONFIG_y0 = 0;
};
