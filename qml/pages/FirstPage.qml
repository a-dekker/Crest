import QtQuick 2.0
import Sailfish.Silica 1.0
import PS 1.0

Page {
    id: page

    property var cpu_size:  (page.width / 10) * 2
    property var rss_size:  (page.width / 10) * 3
    property var name_size: (page.width / 10) * 5
    property var sort: "cpu"
    property var gui_only: true
    function refresh() {
        list.model = ps.get_ps_by(page.sort, page.gui_only);
    }

    PS {
        id: ps
    }
    Timer {
        id: timer
        interval: 3000
        running: true
        repeat: true
        onTriggered: page.refresh()
    }

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: "Sort by: CPU"
                onClicked: { page.sort = "cpu"; page.refresh() }
            }
            MenuItem {
                text: "Sort by: RSS"
                onClicked: { page.sort = "rss"; page.refresh() }
            }
            MenuItem {
                text: "Sort by: Name"
                onClicked: { page.sort = "name"; page.refresh() }
            }
            MenuItem {
                id: guiMenu
                text: "Only GUI: On"
                onClicked: { if(guiMenu.text == "Only GUI: On") {
                                    guiMenu.text = "Only GUI: Off"
                                    page.gui_only = false
                                } else {
                                    guiMenu.text = "Only GUI: On"
                                    page.gui_only = true
                                }
                                page.refresh()
                           }
            }
            MenuItem {
                id: refreshMenu
                text: "Autorefresh: On"
                onClicked: { if(refreshMenu.text == "Autorefresh: On") {
                                    timer.running = false
                                    refreshMenu.text = "Autorefresh: Off"
                                } else {
                                    timer.running = true
                                    refreshMenu.text = "Autorefresh: On"
                                }
                           }
            }
        }
        SilicaListView {
            anchors.fill: parent
            id: list
            model: ps.get_ps_by(page.sort, page.gui_only)
            header: Row {
                spacing: Theme.paddingMedium
                Label {
                    id: cpu
                    text: "CPU"
                    font.bold: true
                    color: Theme.secondaryColor
                    width: page.cpu_size
                    horizontalAlignment: Text.AlignHCenter;
                }
                Label {
                    id: rss
                    text: "RSS"
                    font.bold: true
                    color: Theme.secondaryColor
                    width: page.rss_size
                    horizontalAlignment: Text.AlignHCenter;
                }
                Label {
                    id: name
                    text: "Process"
                    font.bold: true
                    color: Theme.secondaryColor
                    width: page.name_size
                    horizontalAlignment: Text.AlignHCenter;
                }
            }
            delegate: Row {
                spacing: Theme.paddingMedium
                Label { horizontalAlignment: Text.AlignRight; text: modelData["cpu"];  width: page.cpu_size }
                Label { horizontalAlignment: Text.AlignRight; text: modelData["rss"]; width: page.rss_size }
                Label { text: modelData["name"]; width: page.name_size }
            }
        }
    }
}


