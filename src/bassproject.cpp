#include "bassproject.h"
#include <QDateTime>

BassProject::BassProject(bassproject::ProjectMetaPackage metaPackage)
    : _projMetaData(metaPackage), _defaultConstructed(false) {
  // Do initializtion of all sub-modules here
}

BassProject::BassProject() : _defaultConstructed(true) {}

bool BassProject::wasDefaultConstructed() { return _defaultConstructed; }
