import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: settingsPage

    SilicaFlickable {
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: col.height

        clip: true

        VerticalScrollDecorator {}

        Column {
            id: col
            spacing: isPortrait ? Theme.paddingLarge : Theme.paddingMedium
            width: parent.width
            PageHeader {
                title: qsTr("Settings")
            }

            SectionHeader {
                text: qsTr("Preferred startup config")
            }

            ComboBox {
                id: autorefresh
                width: parent.width
                label: qsTr("Autorefresh")
                description: qsTr("Refresh processlist every 3 seconds")
                currentIndex: config.autorefresh ? 0 : 1
                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("On")
                    }
                    MenuItem {
                        text: qsTr("Off")
                    }
                }
                onCurrentItemChanged: {
                    if (autorefresh.currentIndex === 0) {
                        config.autorefresh = true
                    } else
                        config.autorefresh = false
                }
            }
            ComboBox {
                id: processlist
                width: parent.width
                label: qsTr("Process list view")
                description: qsTr("What list to show")
                currentIndex: config.processlist
                              === "gui_only" ? 0 : config.processlist === "all_procs" ? 1 : 2
                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("Only apps")
                    }
                    MenuItem {
                        text: qsTr("All processes")
                    }
                    MenuItem {
                        text: qsTr("Incl. no cmdline [top 60]")
                    }
                }
                onCurrentItemChanged: {
                    if (processlist.currentIndex === 0) {
                        config.processlist = "gui_only"
                    } else if (processlist.currentIndex === 1) {
                        config.processlist = "all_procs"
                    } else if (processlist.currentIndex === 2) {
                        config.processlist = "incl_nocmd"
                    }
                }
            }
            ComboBox {
                id: sortorder
                width: parent.width
                label: qsTr("Sort order")
                description: qsTr("Where to sort list by")
                currentIndex: config.sortorder === "cpu" ? 0 : config.sortorder === "rss" ? 1 : 2
                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("CPU usage")
                    }
                    MenuItem {
                        text: qsTr("RSS memory usage")
                    }
                    MenuItem {
                        text: qsTr("Process name")
                    }
                }
                onCurrentItemChanged: {
                    if (sortorder.currentIndex === 0) {
                        config.sortorder = "cpu"
                    } else if (sortorder.currentIndex === 1) {
                        config.sortorder = "rss"
                    } else if (sortorder.currentIndex === 2) {
                        config.sortorder = "name"
                    }
                }
            }
        }
    }
}
