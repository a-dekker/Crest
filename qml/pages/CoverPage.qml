

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
    property string memTotal
    property string memAvail
    property string swapTotal
    property string swapFree
    property string swapUsage

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
            var meminfo = ps.memory_values()
            memTotal = meminfo["MemTotal"]
            memAvail = meminfo["MemAvail"]
            swapTotal = meminfo["SwapTotal"]
            swapFree = meminfo["SwapFree"]
            swapUsage = swapTotal - swapFree
        }
    }

    Label {
        id: coverHeader
        text: "Crest"
        width: parent.width - Theme.paddingLarge
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        horizontalAlignment: Text.Center
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

    SilicaFlickable {
        width: parent.width
        anchors.top: coverHeaderSeparator.bottom
        anchors.topMargin: Theme.paddingMedium
        anchors.leftMargin: Theme.paddingLarge
        anchors.rightMargin: Theme.paddingLarge
        HighlightImage {
            id: ramIcon
            x: Theme.paddingMedium
            color: Theme.primaryColor
            source: "../icons/ram.png"
            fillMode: Image.PreserveAspectFit
            width: parent.width / 5
        }
        ProgressBar {
            id: memUsageBar
            width: parent.width - ramIcon.width - 2 * Theme.paddingLarge
            anchors.left: ramIcon.right
            anchors.verticalCenter: ramIcon.verticalCenter
            minimumValue: 0
            maximumValue: memTotal
            progressValue: memTotal - memAvail
            leftMargin: Theme.paddingLarge
            rightMargin: 0
            value: memAvail
        }
        Row {
            id: mem_usage
            anchors.top: ramIcon.bottom
            x: Theme.paddingMedium
            y: Theme.paddingMedium
            width: parent.width
            Label {
                width: (parent.width - (Theme.paddingMedium * 2)) * .75
                text: Math.round((memTotal - memAvail) / memTotal * 100) + "%"
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.highlightColor
            }
            Label {
                text: (memTotal - memAvail).toFixed(2) + "G/" + Math.round(
                          memTotal * 100) / 100 + "G"
                color: Theme.highlightColor
                width: (parent.width - (Theme.paddingMedium * 2)) * .25
                horizontalAlignment: Text.AlignRight
                font.pixelSize: Theme.fontSizeSmall
            }
        }
        HighlightImage {
            anchors.top: mem_usage.bottom
            id: swpIcon
            x: Theme.paddingMedium
            color: Theme.primaryColor
            source: "../icons/swap.png"
            width: parent.width / 5
            fillMode: Image.PreserveAspectFit
        }
        ProgressBar {
            id: swapUsageBar
            width: parent.width - swpIcon.width - 2 * Theme.paddingLarge
            anchors.left: swpIcon.right
            anchors.verticalCenter: swpIcon.verticalCenter
            minimumValue: 0
            maximumValue: swapTotal
            progressValue: swapUsage
            leftMargin: Theme.paddingLarge
            rightMargin: 0
            value: swapUsage
            visible: swapTotal !== "0"
        }
        Row {
            id: swap_usage
            anchors.top: swpIcon.bottom
            x: Theme.paddingMedium
            y: Theme.paddingMedium
            width: parent.width
            Label {
                width: (parent.width - (Theme.paddingMedium * 2)) * .75
                text: swapTotal === "0" ? qsTr("No swap") : Math.round(
                                              swapUsage / swapTotal * 100) + "%"
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.highlightColor
            }
            Label {
                text: swapUsage + "M/" + swapTotal + "M"
                color: Theme.highlightColor
                width: (parent.width - (Theme.paddingMedium * 2)) * .25
                horizontalAlignment: Text.AlignRight
                font.pixelSize: Theme.fontSizeSmall
                visible: swapTotal !== "0"
            }
        }
        Column {
            id: col
            width: parent.width
            spacing: Theme.paddingSmall
            anchors.top: swap_usage.bottom
            Row {
                x: Theme.paddingMedium
                y: Theme.paddingMedium
                width: parent.width
                Label {
                    width: (parent.width - (Theme.paddingMedium * 2)) * .75
                    text: qsTr("Load avg:")
                    font.pixelSize: Theme.fontSizeSmall
                }
                Label {
                    id: loadAvg
                    text: ""
                    color: Theme.highlightColor
                    width: (parent.width - (Theme.paddingMedium * 2)) * .25
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: Theme.fontSizeSmall
                }
            }
            Label {
                id: uptimeLabel
                text: qsTr("uptime:")
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: Theme.fontSizeMedium
            }
            Label {
                id: uptime
                color: Theme.highlightColor
                anchors.horizontalCenter: parent.horizontalCenter
                text: ""
                font.pixelSize: Theme.fontSizeSmall
            }
            Label {
                id: uptime_human
                color: Theme.highlightColor
                anchors.horizontalCenter: parent.horizontalCenter
                text: ""
                font.pixelSize: Theme.fontSizeSmall
            }
        }
    }
}
