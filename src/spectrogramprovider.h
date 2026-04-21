#include "audioFilesModel.h"
#include "datarasterfactory.h"
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

public:
  explicit SpectrogramProvider(QQuickItem *parent = nullptr);

  void paint(QPainter *painter) override;

  double xscale() const { return CONFIG_xscale; };
  double yscale() const { return CONFIG_yscale; };
  double color_scale() const { return CONFIG_color_scale; };
  double x0() const { return CONFIG_x0; };
  double y0() const { return CONFIG_y0; };

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
  Q_INVOKABLE void loadNewSpectrogramData(int AudioFilesModelIndex);

  AudioFilesModel *audioFilesModel() const { return m_audioFilesModel; }
  void setAudioFilesModel(AudioFilesModel *model) {
    if (m_audioFilesModel == model)
      return;
    m_audioFilesModel = model;
    emit audioFilesModelChanged();
  }
signals:
  void renderingStarted();
  void renderingFinished();
  void dataLoaded();
  void audioFilesModelChanged();

  void RenderConfigChanged();

private:
  QwtPlotSpectrogram *m_spectrogram = new QwtPlotSpectrogram();
  AudioFilesModel *m_audioFilesModel;
  QwtScaleMap m_xMap;
  QwtScaleMap m_yMap;
  double CONFIG_xscale = 1;
  double CONFIG_yscale = 1;
  double CONFIG_color_scale;
  double CONFIG_x0;
  double CONFIG_y0;
};
