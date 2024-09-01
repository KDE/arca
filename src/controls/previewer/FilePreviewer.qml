import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.mauikit.controls as Maui

import org.mauikit.filebrowsing as FB

Maui.PopupPage
{
    id: control
    implicitHeight: 1000
    property url currentUrl: ""
    property var iteminfo: FB.FM.getFileInfo(control.currentUrl)

    hint: 1
    maxWidth: 800
    maxHeight: implicitHeight

    // title: iteminfo.label

    stack: Loader
    {
        id: previewLoader
        Layout.fillHeight: true
        Layout.fillWidth: true
        asynchronous: true
        active: control.visible
        source: show()
    }

    function show()
    {
        var source = "DefaultPreview.qml"
        if(FB.FM.checkFileType(FB.FMList.AUDIO, iteminfo.mime))
        {
            source = "AudioPreview.qml"
        }else if(FB.FM.checkFileType(FB.FMList.VIDEO, iteminfo.mime))
        {
            source = "VideoPreview.qml"
        }else if(FB.FM.checkFileType(FB.FMList.TEXT, iteminfo.mime))
        {
            source = "TextPreview.qml"
        }else if(FB.FM.checkFileType(FB.FMList.IMAGE, iteminfo.mime))
        {
            source = "ImagePreview.qml"
        }else if(FB.FM.checkFileType(FB.FMList.DOCUMENT, iteminfo.mime))
        {
            source = "DocumentPreview.qml"
        }else if(FB.FM.checkFileType(FB.FMList.FONT, iteminfo.mime))
        {
            source = "FontPreviewer.qml"
        }else
        {
            source = "DefaultPreview.qml"
        }

        return source;
    }


}
