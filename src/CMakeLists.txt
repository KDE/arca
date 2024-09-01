# Copyright 2018-2021 Camilo Higuita <milo.h@aol.com>
# Copyright 2018-2021 Nitrux Latinoamericana S.C.
#
# SPDX-License-Identifier: GPL-3.0-or-later

remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_BYTEARRAY -DQT_NO_URL_CAST_FROM_STRING)

set(project_SRCS
    code/main.cpp
    code/arca.cpp)

qt_add_resources(project_QRC  assets/assets.qrc)

qt_add_executable(${PROJECT_NAME}
    ${project_SRCS}
    ${project_QRC})

qt_add_qml_module(${PROJECT_NAME} URI app.maui.arca
    NO_PLUGIN
    QML_FILES
    controls/main.qml
    controls/previewer/VideoPreview.qml
    controls/previewer/AudioPreview.qml
    controls/previewer/DocumentPreview.qml
    controls/previewer/ImagePreview.qml
    controls/previewer/FontPreviewer.qml
    controls/previewer/TextPreview.qml
    controls/previewer/FilePreviewer.qml
    controls/previewer/DefaultPreview.qml
    controls/previewer/FileProperties.qml)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    MauiKit4
    MauiKit4::FileBrowsing
    MauiKit4::Archiver
    Qt6::Core
    Qt6::Quick
    Qt6::Widgets

    KF6::Archive
    KF6::CoreAddons
    KF6::I18n)

install(TARGETS ${PROJECT_NAME} ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
