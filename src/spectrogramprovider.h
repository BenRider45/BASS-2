#ifndef SPECTROGRAMPROVIDER_H
#define SPECTROGRAMPROVIDER_H

#include <QObject>
#include <QQuickImageProvider>
#include <QImage>
#include <QQmlEngine>

// Image provider: returns placeholder spectrogram/trace images
class SpectrogramProvider : public QQuickImageProvider
{
public:
    SpectrogramProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
};

// Controller exposed to QML for spectrogram navigation and display settings
class SpectrogramController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int navPage READ navPage WRITE setNavPage NOTIFY navPageChanged)
    Q_PROPERTY(int navMax READ navMax NOTIFY navMaxChanged)
    Q_PROPERTY(double vmin READ vmin WRITE setVmin NOTIFY colorLimitsChanged)
    Q_PROPERTY(double vmax READ vmax WRITE setVmax NOTIFY colorLimitsChanged)
    Q_PROPERTY(double threshold READ threshold WRITE setThreshold NOTIFY thresholdChanged)

public:
    explicit SpectrogramController(QObject *parent = nullptr);

    int navPage() const;
    void setNavPage(int page);
    int navMax() const;
    double vmin() const;
    void setVmin(double v);
    double vmax() const;
    void setVmax(double v);
    double threshold() const;
    void setThreshold(double t);

    Q_INVOKABLE void nextPage();
    Q_INVOKABLE void prevPage();
    Q_INVOKABLE void setColorLimits(double min, double max);

signals:
    void navPageChanged();
    void navMaxChanged();
    void colorLimitsChanged();
    void thresholdChanged();

private:
    int m_navPage = 0;
    int m_navMax = 0;
    double m_vmin = 0.0;
    double m_vmax = 1.0;
    double m_threshold = 0.5;
};

#endif // SPECTROGRAMPROVIDER_H
