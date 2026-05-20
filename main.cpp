#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "annotationmodel.h"
#include "audioplayer.h"
#include "bassproject.h"
#include "filelistmodel.h"
#include "projectmanager.h"
#include "sharedconstants.h"
#include "spectrogramprovider.h"
#include <QDirIterator>
int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  QDirIterator it(":", QDirIterator::Subdirectories);
  //  while (it.hasNext())
  //    qDebug() << it.next();
  app.setApplicationName("BASS");
  app.setOrganizationName("JinGroup");

  QQuickStyle::setStyle("Material");

  QQmlApplicationEngine engine;

  // Create and register singleton instances
  ProjectManager projectManager;
  FileListModel fileListModel;
  AudioPlayer audioPlayer;
  engine.rootContext()->setContextProperty("projectManager", &projectManager);
  engine.rootContext()->setContextProperty("fileListModel", &fileListModel);
  engine.rootContext()->setContextProperty("audioPlayer", &audioPlayer);
  qmlRegisterType<SpectrogramProvider>("BASSGraphics", 1, 0,
                                       "SpectrogramProvider");

  qmlRegisterSingletonType<constants::SharedConstants>(
      "Constants", 1, 0, "SharedConstants",
      &constants::SharedConstants::singletonProvider);
  qmlRegisterType<bassproject::projectMetaPackage>("BASS", 1, 0,
                                                   "projectMetaPackage");

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  engine.loadFromModule("BASS", "Main");

  return app.exec();
}
