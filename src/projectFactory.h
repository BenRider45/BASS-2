#pragma once
#include "bassproject.h"

namespace ProjectFactory {

std::unique_ptr<BassProject> createProjectFromMetaData(QDir projectDirectory);

void buildProjectDirectoryStructure(QDir projectDirectory);
std::unique_ptr<BassProject>
createProjectFromNew(bassproject::ProjectMetaPackage projectDirectory);
}; // namespace ProjectFactory
