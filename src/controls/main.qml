import QtQuick 2.15
import QtQml 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12

import org.mauikit.controls 1.3 as Maui
import org.mauikit.filebrowsing 1.3 as FM

import org.kde.arca 1.0 as Arca

import "previewer"

Maui.ApplicationWindow
{
    id: root
    title: qsTr("Arca")

    property alias dialog : _dialogLoader.item


    Loader
    {
        id: _dialogLoader
    }

    Component
    {
        id: _fileDialogComponent

        FM.FileDialog
        {

        }
    }

    Component
    {
        id: _previewComponent

        FilePreviewer
        {
            id: _previewer
        }
    }

    Maui.TabView
    {
        id: _tabView
        anchors.fill: parent
        tabBar.visible: true

tabBar.showNewTabButton: false
        holder.visible: count === 0
        holder.emoji: "archive-insert"
        holder.title: i18n("Compress")
        holder.body: "Drop files in here to compress them."


        holder.actions: [

            Action
            {
                text: i18n("Open archive")
                onTriggered: root.openFileDialog()
            },

            Action
            {
                text: i18n("Compress files")
            }

        ]

        tabBar.rightContent: [

        Maui.ToolButtonMenu
            {
                icon.name: "list-add"

                MenuItem
                {
                    text: i18n("Open")
                    onTriggered: openFileDialog()
                }

                MenuItem
                {
                    text: i18n("Create")
                }
            },

             Maui.WindowControls {}

        ]

        tabBar.leftContent: Maui.ToolButtonMenu
            {
                icon.name: "application-menu"

                MenuItem
                {
                    text: i18n("Open")
                    icon.name: "folder-open"
                    onTriggered: openFileDialog()

                }

                MenuItem
                {
                    text: i18n("Settings")
                    icon.name: "settings-configure"
                    onTriggered: openSettingsDialog()
                }

                MenuItem
                {
                    text: i18n("About")
                    icon.name: "documentinfo"
                    onTriggered: root.about()
                }
            }
    }

    Component
    {
        id: _archivePageComponent

        ArchivePage
        {

            Maui.TabViewInfo.tabTitle: title
            Maui.TabViewInfo.tabToolTipText:  url
            height: ListView.view.height
                width: ListView.view.width
        }
    }

    function openArchive(url)
    {
        _tabView.addTab(_archivePageComponent, {'url': url})
    }

    function previewFile(url)
    {
        _dialogLoader.sourceComponent = _previewComponent
        dialog.currentUrl = url
        dialog.open()
    }

    function openFileDialog()
    {
        _dialogLoader.sourceComponent = _fileDialogComponent
        dialog.mode = dialog.modes.OPEN
        dialog.settings.filterType= FM.FMList.COMPRESSED
        dialog.callback = (paths) => {

            for(var path of paths)
            {
                openArchive(path)
            }
        }

        dialog.open()
    }

}
