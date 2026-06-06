cmake_minimum_required(VERSION 3.19)
project(BASS_QML VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake"  )
include(FindQwt)

find_package(Qt6 REQUIRED COMPONENTS Core Quick QuickControls2 Multimedia)
find_package(Qwt REQUIRED)
set(QT_QML_GENERATE_QMLLS_INI ON)

qt_policy(SET QTP0004 NEW)
qt_standard_project_setup(REQUIRES 6.5)

option(BUILD_TESTS "Build unit tests" ON)
set(SOURCES
    src/projectmanager.cpp
    src/annotationmodel.cpp
    src/spectrogramprovider.cpp
    src/filelistmodel.cpp
    src/audioplayer.cpp
    src/metamanager.cpp
    src/projectFactory.cpp
    src/bassproject.cpp
    src/recentprojectslistmodel.cpp
    src/wavsurfer.cpp
    src/audioFilesModel.cpp
    src/datarasterfactory.cpp
)

set(HEADERS
    src/projectmanager.h
    src/annotationmodel.h
    src/spectrogramprovider.h
    src/filelistmodel.h
    src/audioplayer.h 
    src/sharedconstants.h
    src/metamanager.h
    src/projectFactory.h 
    src/bassproject.h 
    src/errorinator.h
    src/result.h 
    src/serializationtraits.h
    src/recentProject.h
    src/recentprojectslistmodel.h
    src/wavfile.h 
    src/wavsurfer.h 
    src/audioFilesModel.h
    src/fftforeman.h 
    src/Irastercompatibledata.h 
    src/datarasterfactory.h 
    src/spectrogramdatarasterinstance.h 
  )

  set(EXTERNAL_DEPENDENCIES 
    src/dependencies/AudioFile/AudioFile.h
  )
# Create a library from the core source files
add_library(bass_core STATIC ${SOURCES} ${HEADERS} ${EXTERNAL_DEPENDENCIES})

target_include_directories(bass_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(bass_core BEFORE PRIVATE ${QWT_INCLUDE_DIR})

target_link_libraries(bass_core PRIVATE
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Multimedia
    Qt6::Core
)



# Link Qwt library (already found by find_package above)
target_link_libraries(bass_core PRIVATE Qwt::Qwt)
# Create the executable with QML support
qt_add_executable(bass_qml main.cpp)

qt_add_qml_module(bass_qml
    URI BASS
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/SpectrogramView.qml
        qml/AnnotationOverlay.qml
        qml/InfoBar.qml
        qml/SideDrawer.qml
        qml/panels/ControlPanel.qml
        qml/panels/FilesPanel.qml
        qml/panels/SettingsPanel.qml
        qml/panels/PlayerPanel.qml
        qml/panels/ClusterSummaryPanel.qml
        qml/dialogs/LoadingSplashScreen.qml
        qml/dialogs/ProjectLoadConfirmationDialog.qml
        qml/dialogs/ProjectSelectWindow.qml
        qml/dialogs/CreateProjectDialog.qml
        qml/dialogs/ProjectInitDialog.qml
        qml/dialogs/PromptDialog.qml
)

target_include_directories(bass_qml BEFORE PRIVATE ${QWT_INCLUDE_DIR})

target_link_libraries(bass_qml PRIVATE
    bass_core
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Multimedia
)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
