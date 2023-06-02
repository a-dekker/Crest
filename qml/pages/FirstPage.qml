
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
import SortFilterProxyModel 0.2
import "../common"

Page {
    id: page

    property int cpu_size: Theme.fontSizeSmall * 3.4
    property int rss_size: Theme.fontSizeSmall * 4.4
    property int name_size: page.width - cpu_size - rss_size
    property string sort: "cpu"
    property string list_type: "gui_only"
    property bool refreshing: true
    property bool paused: false
    property var applicationActive: app.applicationActive
    property bool pageactive: false
    property string name
    property string name_nopath
    property string cpu
    property int proc_owner
    property string owner_name
    property string pid
    property string ppid
    property string gid
    property string rss
    property string cputime
    property string start_time
    property string mem_perc
    property var hasSudo: ps.hasSudo()

    function refresh() {
        listUnitModel.clear()
        addProc()
        timer.interval = 3000
        page.unpause()
    }
    function pause() {
        if (refreshing) {
            paused = true
            refreshing = false
        }
    }
    function unpause() {
        if (paused) {
            paused = false
            refreshing = true
        }
    }

    function addProc() {
        var data
        data = ps.get_ps_by(page.sort, page.list_type)
        for (var i = 0; i < data.length; i++) {
            listUnitModel.append({
                                     "cpu": data[i]["cpu"],
                                     "proc_owner": data[i]["proc_owner"],
                                     "owner_name": data[i]["owner_name"],
                                     "name": data[i]["name"],
                                     "name_nopath": data[i]["name_nopath"],
                                     "pid": data[i]["pid"],
                                     "gid": data[i]["gid"],
                                     "ppid": data[i]["ppid"],
                                     "rss": data[i]["rss"],
                                     "cputime": data[i]["cputime"],
                                     "start_time": data[i]["start_time"],
                                     "mem_perc": data[i]["mem_perc"]
                                 })
        }
    }

    Component.onCompleted: {
        addProc()
    }
    onApplicationActiveChanged: {
        if (applicationActive) {
            refresh()
        } else {
            pause()
        }
    }

    onStatusChanged: {
        if ((status === PageStatus.Activating)
                || (status === PageStatus.Active)) {
            pageactive = true
        }
    }

    Timer {
        id: timer
        interval: 6000
        running: page.refreshing && page.pageactive
        repeat: true
        onTriggered: page.refresh()
    }

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                id: refreshMenu
                text: qsTr("Autorefresh off")
                onClicked: {
                    page.refreshing = !page.refreshing
                    if (!page.refreshing)
                        refreshMenu.text = qsTr("Autorefresh on")
                    else
                        refreshMenu.text = qsTr("Autorefresh off")
                    page.refresh()
                }
            }
            MenuItem {
                id: guiMenu
                text: qsTr("Show all processes")
                onClicked: {
                    if (page.list_type === "gui_only") {
                        page.list_type = "all_procs"
                        guiMenu.text = "Incl. no cmdline [top 60]"
                    } else if (page.list_type === "all_procs") {
                        page.list_type = "incl_nocmd"
                        guiMenu.text = qsTr("Show only apps")
                    } else if (page.list_type === "incl_nocmd") {
                        guiMenu.text = qsTr("Show all processes")
                        page.list_type = "gui_only"
                    }
                    page.refresh()
                }
            }
            SearchMenuItem {}
            MenuItem {
                id: sortColumn
                text: qsTr("Sort by RSS")
                onClicked: {
                    if (page.sort === "cpu") {
                        page.sort = "rss"
                        sortColumn.text = qsTr("Sort by name")
                    } else if (page.sort === "rss") {
                        page.sort = "name"
                        sortColumn.text = qsTr("Sort by CPU")
                    } else if (page.sort === "name") {
                        page.sort = "cpu"
                        sortColumn.text = qsTr("Sort by RSS")
                    }
                    page.refresh()
                }
            }
        }

        SearchPanel {
            id: searchPanel
        }

        Item {
            id: headerBox
            // attach y position of header box to list view content y position
            // that way the header box moves accordingly when pulley menus are opened
            y: 0 - list.contentY - height
            z: 1
            width: parent.width
            height: pageHeader.height

            PageHeaderExtended {
                id: pageHeader
                anchors.top: parent.top
                anchors.topMargin: Theme.paddingSmall
                anchors.left: parent.left
                Row {
                    Label {
                        id: cpuLabel
                        text: sort == "cpu" ? qsTr("▼ CPU") : sort
                                              == "-cpu" ? qsTr("▲ CPU") : qsTr(
                                                              "CPU")
                        color: Theme.secondaryColor
                        font.bold: true
                        width: page.cpu_size
                        horizontalAlignment: Text.AlignHCenter
                    }
                    Label {
                        text: sort == "rss" ? qsTr("▼ RSS") : sort
                                              == "-rss" ? qsTr("▲ RSS") : qsTr(
                                                              "RSS")
                        color: Theme.secondaryColor
                        font.bold: true
                        width: page.rss_size
                        horizontalAlignment: Text.AlignHCenter
                    }
                    Label {
                        text: sort == "name" ? qsTr("▼ Process") : sort
                                               == "-name" ? qsTr("▲ Process") : qsTr(
                                                                "Process")
                        color: Theme.secondaryColor
                        font.bold: true
                        width: page.name_size
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }

        SilicaListView {
            id: list

            property Item contextMenu

            clip: true
            x: Theme.paddingSmall
            y: Theme.paddingMedium
            width: parent.width - 2 * x
            height: parent.height - y
            model: listUnitProxyModel
            onMovementStarted: page.pause()
            onMovementEnded: page.unpause()

            anchors {
                fill: parent
                bottomMargin: 0
                topMargin: searchPanel.visibleSize
            }

            VerticalScrollDecorator {}

            ListModel {
                id: listUnitModel
            }

            SortFilterProxyModel {
                id: listUnitProxyModel
                sourceModel: listUnitModel
                filters: RegExpFilter {
                    roleName: isPortrait ? "name_nopath" : "name"
                    pattern: searchPanel.searchText
                    caseSensitivity: Qt.CaseInsensitive
                }
                sorters: StringSorter {
                    enabled: sort === "name" || sort === "-name"
                    sortOrder: sort === "-name" ? Qt.DescendingOrder : Qt.AscendingOrder
                    roleName: isPortrait ? "name_nopath" : "name"
                }
            }

            header: Item {
                id: header

                // This is just a placeholder for the header box. To avoid the
                // list view resetting the input box everytime the model resets,
                // the search entry is defined outside the list view.
                width: pageHeader.width
                height: pageHeader.height
                Component.onCompleted: pageHeader.parent = header
            }

            delegate: ListItem {
                menu: hasSudo || proc_owner === 100000 ? contextMenu : null
                onClicked: {
                    showProcDetails()
                }
                contentHeight: Theme.fontSizeSmall * 1.5
                Row {
                    spacing: Theme.paddingMedium
                    Label {
                        horizontalAlignment: Text.AlignRight
                        text: cpu
                        width: page.cpu_size
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.highlightColor
                    }
                    Label {
                        horizontalAlignment: Text.AlignRight
                        text: rss
                        width: page.rss_size
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.secondaryColor
                    }
                    Label {
                        text: isPortrait ? Theme.highlightText(
                                               name_nopath,
                                               searchPanel.searchText,
                                               Theme.highlightColor) : Theme.highlightText(
                                               name, searchPanel.searchText,
                                               Theme.highlightColor)
                        width: page.name_size
                        truncationMode: TruncationMode.Fade
                        font.pixelSize: Theme.fontSizeSmall
                        color: proc_owner === 100000 ? Theme.secondaryColor : (proc_owner === 0 ? Theme.primaryColor : Theme.secondaryHighlightColor)
                    }
                }

                function showProcDetails() {
                    pageactive = false
                    pageStack.push(Qt.resolvedUrl("ProcInfo.qml"), {
                                       "cpu": cpu,
                                       "proc_owner": proc_owner,
                                       "owner_name": owner_name,
                                       "name": name,
                                       "pid": pid,
                                       "ppid": ppid,
                                       "gid": gid,
                                       "start_time": start_time,
                                       "rss": rss,
                                       "cputime": cputime,
                                       "mem_perc": mem_perc
                                   })
                }

                function killRemorse(kill_signal) {
                    remorseAction(qsTr(""), function () {
                        ps.killproc(pid, kill_signal, proc_owner)
                        page.refresh()
                    }, 3000)
                }

                Component {
                    id: contextMenu
                    ContextMenu {
                        Component.onCompleted: page.pause()
                        onClosed: page.unpause()
                        MenuItem {
                            text: qsTr("Terminate pid ") + pid
                            onClicked: {
                                killRemorse(15)
                            }
                        }
                        MenuItem {
                            text: qsTr("Force Kill pid ") + pid
                            onClicked: {
                                killRemorse(9)
                            }
                        }
                        MenuItem {
                            text: qsTr("Process info")
                            onClicked: {
                                showProcDetails()
                            }
                        }
                    }
                }
            }
        }
    }
}
