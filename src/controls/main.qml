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

    Maui.Dialog
    {
        id: _newArchiveDialog
        persistent: false

        acceptButton.text: i18n("Create")
        onRejected: close()

        TextField
        {
            Layout.fillWidth: true
            placeholderText: i18n("Archive name...")
        }

        TextField
        {
            Layout.fillWidth: true
            placeholderText: i18n("Location")
            text: Arca.Arc.defaultSaveDir
        }

        Maui.ToolActions
        {
            id: compressType
            autoExclusive: true
            expanded: true
            display: ToolButton.TextBesideIcon

            Action
            {
                text: ".ZIP"
                icon.name:  "application-zip"
            }

            Action
            {
                text: ".TAR"
                icon.name:  "application-x-tar"

            }

            Action
            {
                text: ".7ZIP"
                icon.name:  "application-x-rar"

            }
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
                id: _openArchiveAction
                icon.name: "folder-open"
                text: i18n("Open archive")
                onTriggered: root.openFileDialog()
            },

            Action
            {
                id: _createArchiveAction
                text: i18n("Compress files")
                icon.name: "archive-insert"
                onTriggered: _newArchiveDialog.open()
            }

        ]

        tabBar.rightContent: [

            Maui.ToolButtonMenu
            {
                icon.name: "list-add"

                MenuItem
                {
                    action: _openArchiveAction
                }

                MenuItem
                {
                    action:_createArchiveAction
                }

                MenuSeparator{}

                MenuItem
                {
                    text: i18n("About")
                    icon.name: "documentinfo"
                    onTriggered: root.about()
                }
            },

            Maui.WindowControls {}

        ]
    }

    Component
    {
        id: _archivePageComponent

        ArchivePage
        {
            Maui.TabViewInfo.tabTitle: title
            Maui.TabViewInfo.tabToolTipText:  url
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
