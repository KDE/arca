import QtQuick
import QtQml
import QtQuick.Controls
import QtQuick.Layouts

import org.mauikit.controls as Maui
import org.mauikit.filebrowsing as FM
import org.mauikit.archiver as Arc

import org.kde.arca as Arca

import "previewer"

Maui.ApplicationWindow
{
    id: root
    title: qsTr("Arca")

    property alias dialog : _dialogLoader.item
    property alias currentTab: _tabView.currentItem

    Loader
    {
        id: _dialogLoader
    }

    Component
    {
        id: _fileDialogComponent

        FM.FileDialog {}
    }

    Component
    {
        id: _previewComponent

        FilePreviewer
        {
            id: _previewer
        }
    }

    NewArchiveDialog
    {
        id: _newArchiveDialog
        onDone:
        {
           var tab = _tabView.addTab(_archivePageComponent, ({}))
           tab.create(files, path, name, type)
            _newArchiveDialog.close()
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

        Maui.Controls.showCSD: true
        onCloseTabClicked: _tabView.closeTab(index)

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
                onTriggered:
                {
                    _dialogLoader.sourceComponent = _fileDialogComponent
                    dialog.mode = FM.FileDialog.Open
                    dialog.browser.settings.filterType = FM.FMList.NONE
                    dialog.callback = (paths) => {

                        _newArchiveDialog.urls = paths
                        _newArchiveDialog.open()
                    }

                    dialog.open()
                }
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
            }
        ]
    }

    Component
    {
        id: _archivePageComponent

        Arc.ArchivePage
        {
            Maui.Controls.title: title
            Maui.Controls.toolTipText:  url
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
        dialog.mode = FM.FileDialog.Open
        dialog.browser.settings.filterType = FM.FMList.COMPRESSED
        dialog.callback = (paths) => {

            for(var path of paths)
            {
                openArchive(path)
            }
        }

        dialog.open()
    }

}
