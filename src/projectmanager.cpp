#include "projectmanager.h"

ProjectManager::ProjectManager(QObject *parent)
    : QObject(parent)
{
}

QString ProjectManager::birdName() const { return m_birdName; }
QString ProjectManager::projectDir() const { return m_projectDir; }
QString ProjectManager::wavDir() const { return m_wavDir; }
bool ProjectManager::isInitialized() const { return m_isInitialized; }

void ProjectManager::initProject(const QString &projectDir, const QString &birdName)
{
    m_projectDir = projectDir;
    m_birdName = birdName;
    m_isInitialized = true;
    emit projectLoaded();
}

void ProjectManager::setWavDir(const QString &path)
{
    m_wavDir = path;
    emit projectLoaded();
}
