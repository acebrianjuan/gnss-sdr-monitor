// clang-format off

import QtQuick 2.0
import QtQuick.Window 2.0
import QtLocation 5.6
import QtPositioning 5.6
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Map
{
    property variant cttc: QtPositioning.coordinate(41.27504, 1.987709)

    id: map
    height: 300

    plugin: Plugin
    {
        id: mapPlugin
        name: "esri"

        PluginParameter
        {
            name: "esri.mapping.maximumZoomLevel"
            value: 19.90
        }
    }
    center: cttc
    zoomLevel: 15

    MapPolyline // Path of the vehicle.
    {
        line.width: 3
        line.color: "red"
        opacity: 0.3
        path: m_monitor_pvt_wrapper.path
        visible: show_path.checked
    }

    MapQuickItem // Current position of the vehicle.
    {
        id: vehicle
        coordinate: m_monitor_pvt_wrapper.position

        anchorPoint.x: icon.width/2
        anchorPoint.y: icon.height/2

        sourceItem: Rectangle
        {
            id: icon
            width: 10
            height: 10
            radius: width/2
            color: "red"
        }

    }

    ColumnLayout
    {
        CheckBox // Show Path CheckBox.
        {
            id: show_path
            checked: true
            text: "Show Path"
        }

        CheckBox // Follow CheckBox.
        {
            id: follow
            checked: true
            text: "Follow"
        }
    }

    Connections
    {
        target: vehicle
        onCoordinateChanged:
        {
            if (follow.checked)
            {
                map.center = m_monitor_pvt_wrapper.position;
            }
        }
    }
}
