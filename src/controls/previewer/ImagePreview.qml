import QtQuick
import QtQuick.Controls

import org.mauikit.controls as Maui

Item
{
    id: control
    Loader
    {
        anchors.fill: parent
        asynchronous: true
        sourceComponent: iteminfo.mime === "image/gif" || iteminfo.mime === "image/avif" ? _animatedImgComponent : _imgComponent

        Component
        {
            id: _animatedImgComponent
            Maui.AnimatedImageViewer
            {
                source: currentUrl
            }
        }

        Component
        {
            id: _imgComponent
            Maui.ImageViewer
            {
                source: currentUrl
            }
        }
    }


}


