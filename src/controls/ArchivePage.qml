import QtQuick 2.15
import QtQml 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12

import org.mauikit.controls 1.3 as Maui
import org.mauikit.filebrowsing 1.3 as FM

import org.kde.arca 1.0 as Arca

Maui.Page
{

    id: control
    title: _manager.model.fileName || root.title
    property alias url : _manager.url
showTitle: false

    Arca.CompressedFile
    {
        id: _manager
    }

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

    Maui.ListBrowser
    {
        id: _browser
        anchors.fill: parent
        model: Maui.BaseModel
        {
            list: _manager.model
        }


        holder.visible: _browser.count === 0
        holder.emoji: "archive-insert"
        holder.title: i18n("Compress")
        holder.body: "Drop files in here to compress them."



        delegate: Maui.ListBrowserDelegate
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
            var url = _manager.model.temporaryFile(item.path)
            previewFile(url)
        }
    }

}
