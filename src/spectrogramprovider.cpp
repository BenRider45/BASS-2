#include "spectrogramprovider.h"
#include <QPainter>
#include <QLinearGradient>

// --- SpectrogramProvider ---

SpectrogramProvider::SpectrogramProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage SpectrogramProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    int width = requestedSize.width() > 0 ? requestedSize.width() : 800;
    int height = requestedSize.height() > 0 ? requestedSize.height() : 256;

    QImage img(width, height, QImage::Format_RGB32);

    if (id == "trace") {
        // Placeholder trace: dark background with a horizontal line
        img.fill(QColor(30, 30, 30));
        QPainter p(&img);
        p.setPen(QPen(QColor(0, 200, 100), 1));
        int mid = height / 2;
        p.drawLine(0, mid, width, mid);
    } else {
        // Placeholder spectrogram: dark gradient
        QPainter p(&img);
        QLinearGradient grad(0, 0, 0, height);
        grad.setColorAt(0.0, QColor(10, 10, 40));
        grad.setColorAt(0.5, QColor(30, 30, 80));
        grad.setColorAt(1.0, QColor(10, 10, 40));
        p.fillRect(img.rect(), grad);

        // Draw a grid to make it look like a spectrogram placeholder
        p.setPen(QPen(QColor(60, 60, 100), 1, Qt::DotLine));
        for (int y = 0; y < height; y += 32)
            p.drawLine(0, y, width, y);
        for (int x = 0; x < width; x += 80)
            p.drawLine(x, 0, x, height);
    }

    if (size)
        *size = img.size();
    return img;
}

// --- SpectrogramController ---

SpectrogramController::SpectrogramController(QObject *parent)
    : QObject(parent)
{
}

int SpectrogramController::navPage() const { return m_navPage; }
void SpectrogramController::setNavPage(int page)
{
    if (m_navPage != page) {
        m_navPage = page;
        emit navPageChanged();
    }
}

int SpectrogramController::navMax() const { return m_navMax; }

double SpectrogramController::vmin() const { return m_vmin; }
void SpectrogramController::setVmin(double v)
{
    if (!qFuzzyCompare(m_vmin, v)) {
        m_vmin = v;
        emit colorLimitsChanged();
    }
}

double SpectrogramController::vmax() const { return m_vmax; }
void SpectrogramController::setVmax(double v)
{
    if (!qFuzzyCompare(m_vmax, v)) {
        m_vmax = v;
        emit colorLimitsChanged();
    }
}

double SpectrogramController::threshold() const { return m_threshold; }
void SpectrogramController::setThreshold(double t)
{
    if (!qFuzzyCompare(m_threshold, t)) {
        m_threshold = t;
        emit thresholdChanged();
    }
}

void SpectrogramController::nextPage()
{
    if (m_navPage < m_navMax) {
        m_navPage++;
        emit navPageChanged();
    }
}

void SpectrogramController::prevPage()
{
    if (m_navPage > 0) {
        m_navPage--;
        emit navPageChanged();
    }
}

void SpectrogramController::setColorLimits(double min, double max)
{
    m_vmin = min;
    m_vmax = max;
    emit colorLimitsChanged();
}
