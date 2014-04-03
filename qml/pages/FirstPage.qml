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

import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page

    allowedOrientations: Orientation.All

    property var cpu_size:  Theme.fontSizeSmall * 3.4
    property var rss_size:  Theme.fontSizeSmall * 4.4
    property var name_size: page.width - cpu_size - rss_size
    property var sort: "cpu"
    property var gui_only: true
    property var refreshing: true
    property var paused: false
    property var applicationActive: app.applicationActive && app.is_ok
    function refresh() {
        list.model = ps.get_ps_by(page.sort, page.gui_only);
        timer.interval = 3000
        page.unpause()
    }
    function pause() {
        if(refreshing) {
            paused = true;
            refreshing = false;
        }
    }
    function unpause() {
        if(paused) {
            paused = false;
            refreshing = true;
        }
    }
    onApplicationActiveChanged: {
        if(applicationActive) {
            refresh();
        } else {
            pause();
        }
    }

    Timer {
        id: timer
        interval: 3000
        running: page.refreshing
        repeat: true
        onTriggered: page.refresh()
    }

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: "Sort by: CPU"
                onClicked: {
                    page.sort = (page.sort == "cpu")?"-cpu":"cpu";
                    page.refresh(); }
            }
            MenuItem {
                text: "Sort by: RSS"
                onClicked: {
                    page.sort = (page.sort == "rss")?"-rss":"rss";
                    page.refresh(); }
            }
            MenuItem {
                text: "Sort by: Name"
                onClicked: {
                    page.sort = (page.sort == "name")?"-name":"name";
                    page.refresh(); }
            }
            MenuItem {
                id: guiMenu
                text: "Apps only: On"
                onClicked: { page.gui_only = !page.gui_only
                                if(!page.gui_only) {
                                    guiMenu.text = "App only: Off"
                                } else {
                                    guiMenu.text = "Apps only: On"
                                }
                                page.refresh()
                           }
            }
            MenuItem {
                id: refreshMenu
                text: "Autorefresh: On"
                onClicked: {  page.refreshing = !page.refreshing
                                if(!page.refreshing) {
                                    refreshMenu.text = "Autorefresh: Off"
                                } else {
                                    refreshMenu.text = "Autorefresh: On"
                                }
                                page.refresh()
                           }
            }
        }
        SilicaListView {
            x: Theme.paddingSmall
            y: Theme.paddingMedium
            width: parent.width - 2*x
            height: parent.height - y
            id: list
            property Item contextMenu;
            model: ps.get_ps_by(page.sort, page.gui_only)
            onMovementStarted: page.pause()
            onMovementEnded: page.unpause()
            header: Row {
                id: header
                spacing: Theme.paddingMedium
                Label {
                    id: cpuLabel
                    text: (sort == "cpu") ? "▼ CPU" : ((sort == "-cpu") ? "▲ CPU" : "CPU")
                    color: Theme.secondaryColor
                    width: page.cpu_size
                    horizontalAlignment: Text.AlignHCenter
                }
                Label {
                    id: rss
                    text: (sort == "rss") ? "▼ RSS" : ((sort == "-rss") ? "▲ RSS" : "RSS")
                    color: Theme.secondaryColor
                    width: page.rss_size
                    horizontalAlignment: Text.AlignHCenter
                }
                Label {
                    id: name
                    text: (sort == "-name") ? "▼ Process" : ((sort == "name") ? "▲ Process" : "Process")
                    color: Theme.secondaryColor
                    width: page.name_size
                    horizontalAlignment: Text.AlignHCenter
                }
            }
            delegate: ListItem {
                menu: modelData["killable"] ? contextMenu : null
                onClicked: { page.pause(); page.unpause(); }
                contentHeight: Theme.fontSizeSmall * 1.5
                Row {
                spacing: Theme.paddingMedium
                Label { horizontalAlignment: Text.AlignRight; text: modelData["cpu"]; width: page.cpu_size; font.pixelSize: Theme.fontSizeSmall; color: modelData["killable"] ? Theme.primaryColor : Theme.secondaryColor }
                Label { horizontalAlignment: Text.AlignRight; text: modelData["rss"]; width: page.rss_size; font.pixelSize: Theme.fontSizeSmall; color: modelData["killable"] ? Theme.primaryColor : Theme.secondaryColor }
                Label { text: modelData["name"]; width: page.name_size; font.pixelSize: Theme.fontSizeSmall; color: modelData["killable"] ? Theme.primaryColor : Theme.secondaryColor }
                }
                Component {
                    id: contextMenu
                    ContextMenu {
                    Component.onCompleted: page.pause()
                    Component.onDestruction: page.unpause()
                        MenuItem {
                            text: "Terminate"
                            font.pixelSize: Theme.fontSizeSmall
                            height: Theme.fontSizeSmall * 2
                            onClicked: { ps.kill(modelData["pid"], 15); page.refresh(); }
                        }
                        MenuItem {
                            text: "Force Kill"
                            height: Theme.fontSizeSmall * 2
                            font.pixelSize: Theme.fontSizeSmall
                            onClicked: { ps.kill(modelData["pid"], 9); page.refresh(); }
                        }
                } }
            }
        }

    }
}


