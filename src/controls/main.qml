import QtQuick 2.15
import QtQml 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12

import org.mauikit.controls 1.3 as Maui
import org.mauikit.filebrowsing 1.3 as FM

Maui.ApplicationWindow
{
    id: root
    title: qsTr("Arca")

    Maui.Page
    {
        anchors.fill: parent
        title: root.title

        Maui.Holder
        {
            anchors.fill: parent
            visible: true
            emoji: "archive-insert"
            title: i18n("Compress")
            body: "Drop files in here to compress them."

            actions: [

                Action
                {
                    text: i18n("Open archive")
                },

                Action
                {
                    text: i18n("Compress files")
                }

            ]
        }
    }

}
