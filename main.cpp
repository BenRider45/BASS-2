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
  AnnotationModel annotationModel;
  SpectrogramController spectrogramController;
  FileListModel fileListModel;
  AudioPlayer audioPlayer;

  engine.rootContext()->setContextProperty("projectManager", &projectManager);
  engine.rootContext()->setContextProperty("annotationModel", &annotationModel);
  engine.rootContext()->setContextProperty("spectrogramController",
                                           &spectrogramController);
  engine.rootContext()->setContextProperty("fileListModel", &fileListModel);
  engine.rootContext()->setContextProperty("audioPlayer", &audioPlayer);

  qmlRegisterSingletonType<constants::SharedConstants>(
      "Constants", 1, 0, "SharedConstants",
      &constants::SharedConstants::singletonProvider);
  qmlRegisterType<bassproject::ProjectMetaPackage>("BASS", 1, 0, "ProjectMetaPackage");
  // Register image provider (engine takes ownership)
  engine.addImageProvider("spectrogram", new SpectrogramProvider);

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  engine.loadFromModule("BASS", "Main");

  return app.exec();
}
