

/******************************************************************************
 *                                                                            *
 * Crest - top like tool for SailfishOS                                       *
 * Copyright (C) 2014 by Michal Hrusecky <Michal@Hrusecky.net>                *
 *                                                                            *
 * This program is free software: you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                            *
 ******************************************************************************/
import QtQuick 2.2
import Sailfish.Silica 1.0

CoverBackground {
    id: coverPage

    property bool active: status === Cover.Active

    Timer {
        id: pageTimer
        interval: 1000
        running: active
        repeat: active
        onTriggered: {
            loadAvg.text = ps.load_avg()
            var tot_uptime = ps.uptime().split("\n")
            uptime.text = tot_uptime[0]
            uptime_human.text = tot_uptime[1]
        }
    }

    Label {
        id: coverHeader
        text: "Crest"
        width: parent.width - Theme.paddingLarge
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        horizontalAlignment: Text.Center
        color: Theme.highlightColor
        font.pixelSize: Theme.fontSizeSmall
    }
    Separator {
        id: coverHeaderSeparator
        color: Theme.primaryColor
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: coverHeader.bottom
        horizontalAlignment: Qt.AlignHCenter
    }

    Column {
        width: parent.width
        spacing: Theme.paddingMedium
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 15
        Label {
            id: loadLabel
            text: qsTr("Load avg")
            anchors.horizontalCenter: parent.horizontalCenter
            color: Theme.secondaryColor
            font.pixelSize: Theme.fontSizeLarge
        }
        Label {
            id: loadAvg
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            text: ""
            font.pixelSize: Theme.fontSizeExtraLarge
        }
        Label {
            id: uptimeLabel
            text: qsTr("Uptime")
            anchors.horizontalCenter: parent.horizontalCenter
            color: Theme.secondaryColor
            font.pixelSize: Theme.fontSizeLarge
        }
        Label {
            id: uptime
            anchors.horizontalCenter: parent.horizontalCenter
            text: ""
            font.pixelSize: Theme.fontSizeSmall
        }
        Label {
            id: uptime_human
            anchors.horizontalCenter: parent.horizontalCenter
            text: ""
            font.pixelSize: Theme.fontSizeSmall
        }
    }
}
