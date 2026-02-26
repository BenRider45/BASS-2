#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QObject>
#include <QString>
#include <QQmlEngine>

class ProjectManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString birdName READ birdName NOTIFY projectLoaded)
    Q_PROPERTY(QString projectDir READ projectDir NOTIFY projectLoaded)
    Q_PROPERTY(QString wavDir READ wavDir NOTIFY projectLoaded)
    Q_PROPERTY(bool isInitialized READ isInitialized NOTIFY projectLoaded)

public:
    explicit ProjectManager(QObject *parent = nullptr);

    QString birdName() const;
    QString projectDir() const;
    QString wavDir() const;
    bool isInitialized() const;

    Q_INVOKABLE void initProject(const QString &projectDir, const QString &birdName);
    Q_INVOKABLE void setWavDir(const QString &path);

signals:
    void projectLoaded();

private:
    QString m_birdName;
    QString m_projectDir;
    QString m_wavDir;
    bool m_isInitialized = false;
};

#endif // PROJECTMANAGER_H
