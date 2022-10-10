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



    Arca.CompressedFile
    {
        id: _manager
    }

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


    Maui.AltBrowser
    {
        id: _browser
        anchors.fill: parent
        model: Maui.BaseModel
        {
            list: _manager.model
        }
        title: _manager.model.fileName || root.title
        showCSDControls: true


        headBar.rightContent: [

            Maui.ToolButtonMenu
            {
              icon.name:  "archive-extract"
              enabled: _manager.model.opened

                MenuItem
                {
                    text: i18n("Extract")
                }

                MenuItem
                {
                    text: i18n("Extract to...")
                }
            },

            ToolButton
            {
                icon.name: "archive-insert"
                enabled: _manager.model.opened
            }
        ]

        headBar.leftContent: [
            Maui.ToolButtonMenu
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
                },

            ToolButton
            {
                icon.name: "go-up"
                onClicked: _manager.model.goUp()
                enabled: _manager.model.canGoUp
            },

            ToolButton
            {
                icon.name: "folder-root"
                onClicked: _manager.model.goToRoot()
                enabled: _manager.model.opened
            }

        ]

        holder.visible: _browser.count === 0
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

        Label
        {
            color: "pink"
            text: _manager.model.currentPath
        }

        listDelegate: Maui.ListBrowserDelegate
        {
            width: ListView.view.width

            label1.text: model.label
            label2.text: model.path
            label3.text: Maui.Handy.formatSize(model.size)
            iconSource: model.icon

            onClicked:
            {
                if(Maui.Handy.singleClick)
                {
                    _browser.currentIndex = index
                    openItem(_browser.model.get(index))
                }

            }

            onDoubleClicked:
            {
                if(!Maui.Handy.singleClick)
                {
                    _browser.currentIndex = index
                    openItem(_browser.model.get(index))
                }
            }

            onRightClicked:
            {
                _browser.currentIndex = index
                _menu.item = _browser.model.get(index)
                _menu.show()
            }


        }

        gridDelegate: Item
        {
            width: GridView.view.cellWidth
            height: GridView.view.cellHeight

            Maui.GridBrowserDelegate
            {
                anchors.fill: parent
                anchors.margins: Maui.Style.space.medium
                iconSource: model.icon
            }
        }

        Maui.ContextualMenu
        {
            id: _menu

            property var item

            MenuItem
            {
                text: i18n("Preview")
                icon.name: "quickopen"
                onTriggered: root.previewFile(_menu.item.path)
            }

            MenuItem
            {
                text: i18n("Open")
                icon.name: "document-open"
            }

            MenuItem
            {
                text: i18n("Open with")
            }

            MenuSeparator{}

            MenuItem
            {
                text: i18n("Delete")
                icon.name: "entry-delete"
            }
        }


    }


    function openItem(item)
    {
        if(item.isdir === "true")
        {
            _manager.model.openDir(item.path)

        }else
        {
            previewFile(item.path)
        }


    }

    function previewFile(path)
    {
        var url = _manager.model.temporaryFile(path)
        _dialogLoader.sourceComponent = _previewComponent
        dialog.currentUrl = url
        dialog.open()

    }

    function openFile(url)
    {
        _manager.url = url
    }

    function openFileDialog()
    {
        _dialogLoader.sourceComponent = _fileDialogComponent
        dialog.mode = dialog.modes.OPEN
        dialog.settings.filterType= FM.FMList.COMPRESSED
        dialog.callback = (paths) => {

            for(var path of paths)
            {
                openFile(path)
            }
        }

        dialog.open()
    }

}
