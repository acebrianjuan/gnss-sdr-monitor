import QtQuick 2.0
import QtQuick.Window 2.0
import QtLocation 5.6
import QtPositioning 5.6

Item
{
    width: 512
    height: 512
    //visible: true
    id: root
    property variant cttc: QtPositioning.coordinate(41.27504, 1.987709)

    Map
    {
        id: map
        anchors.fill: parent
        plugin: Plugin
        {
            id: mapPlugin
            name: "esri"
        }
        center: cttc
        zoomLevel: 15

        MapQuickItem
        {
            id: marker
            coordinate: cttc
            anchorPoint.x: icon.width * 0.5
            anchorPoint.y: icon.height * 0.5

            sourceItem: Rectangle
            {
                id: icon
                width: 10
                height: 10
                radius: width/2
                color: "red"
            }

            function recenter(lat, lon)
            {
                clearMapItems();
                marker.coordinate.latitude = lat;
                marker.coordinate.longitude = lon;
                addMapItem(marker);
                map.center.latitude = lat;
                map.center.longitude = lon;
                map.update();
            }
        }
    }
}
