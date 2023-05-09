import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12

import org.mauikit.controls 1.3 as Maui
import org.mauikit.filebrowsing 1.3 as FM

import org.kde.arca 1.0 as Arca

Maui.FileListingDialog
{
    id: control

    persistent: false

    message: i18n("Create a new archive file.")

    acceptButton.text: i18n("Create")
    onOpened: _archiveNameField.forceActiveFocus()

    onRejected: close()
    onAccepted:
    {
        const ok = control.checkExistance(_archiveNameField.text, _locationField.text, control.extensionName(control.type))

        if(!ok)
        {
            return
        }else
        {
            control.done(control.urls, _locationField.text, _archiveNameField.text, control.type)
//            control.close()
        }
    }

    signal done(var files, string path, string name, int type)

    property int type : 0
    onTypeChanged:
    {
        control.checkExistance(_archiveNameField.text, _locationField.text, control.extensionName(control.type))
    }

    TextField
    {
        id: _archiveNameField
        Layout.fillWidth: true
        placeholderText: i18n("Archive name...")

        onTextChanged:
        {
            control.checkExistance(text, _locationField.text, control.extensionName(control.type))
        }
    }

    TextField
    {
        id: _locationField
        Layout.fillWidth: true
        placeholderText: i18n("Location")
        text: Arca.Arc.defaultSaveDir

        onTextChanged:
        {
            control.checkExistance(_archiveNameField.text, text, control.extensionName(control.type))
        }
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
            checked: control.type === 0
            onTriggered: control.type = 0
        }

        Action
        {
            text: ".TAR"
            icon.name:  "application-x-tar"
            checked: control.type === 1
            onTriggered: control.type = 1
        }

        Action
        {
            text: ".7ZIP"
            icon.name:  "application-x-rar"
            checked: control.type === 2
            onTriggered: control.type = 2
        }
    }

    function checkExistance(name, path, extension)
    {
        if(!FM.FM.fileExists(path))
        {
            control.alert(i18n("Base location does not exists. Try with a different location."), 2)
            return false
        }

        if(name.length === 0)
        {
            control.alert(i18n("File name can not be empty."), 2)
            return fals
        }

        var file = path+"/"+name+extension
        var exists = FM.FM.fileExists(file)

        if(exists)
        {
            control.alert(i18n("File already exists. Try with another name."), 1)
            return false
        }

        control.alert(i18n("Looks good"), 0)
        return true
    }

    function extensionName(type)
    {
        var extension = ""
        switch(control.type)
        {
        case 0: extension = ".zip"; break;
        case 1: extension = ".tar"; break;
        case 2: extension = ".7zip"; break;
        }
        return extension;
    }
}

