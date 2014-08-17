import QtQuick 2.3

import RenderTools 1.0

import "../js/settings.js" as Settings
import "../js/helpers.js" as Helpers

Item {
    id: modelViewerEx;

    property vector2d xRange: Qt.vector2d(0.0, 1.0);
    property vector2d yRange: Qt.vector2d(0.0, 1.0);
    property vector2d zRange: Qt.vector2d(0.0, 1.0);

    property vector3d rotation: Qt.vector3d(0.0, 0.0, 0.0);
    property real zoomFactor: 2.0;

    property int minHU: 0;
    property int maxHU: 65535;

    property string selectedPointName: "";
    property color selectedPointColor: Qt.rgba(0, 0, 0, 0);

    property int modelID: modelViewer.modelID;

    signal pointUpdated(variant point);

    Rectangle {
        ModelViewer {
            id: modelViewer;

            width: modelViewerEx.width;
            height: modelViewerEx.height;
            z: -2;

            fboSize: Qt.size(768, 768);

            xRange: modelViewerEx.xRange;
            yRange: modelViewerEx.yRange;
            zRange: modelViewerEx.zRange;

            rotation: modelViewerEx.rotation;

            zoomFactor: modelViewerEx.zoomFactor;

            minHU: modelViewerEx.minHU;
            maxHU: modelViewerEx.maxHU;

            selectedPointColor: modelViewerEx.selectedPointColor;
            selectedPointName: modelViewerEx.selectedPointName;

            onPointUpdated: {
                // update info about points
                Settings.Points[point.modelID] = Settings.Points[point.modelID] || { };
                Settings.Points[point.modelID][point.name] = Settings.Points[point.modelID][point.name] || { };
                Settings.Points[point.modelID][point.name]["position"] = point.position;

                // need to update sidebar info, so give singal about it
                modelViewerEx.pointUpdated(point);
            }

            function updateViewPortInfo(viewPortArray) {
                //Helpers.createObjects(viewPortArray, Settings.ViewPorts, modelViewer, "/qml/ViewPort.qml");
            }

            ViewportArray {
                id: viewportArray;

                anchors.fill: parent;

                ViewportEx  {
                    projectionType: Viewport.PERSPECTIVE;
                    boundingRect: Qt.rect(0.5, 0.5, 0.5, 0.5);
                }

                ViewportEx {
                    projectionType: Viewport.TOP;
                    boundingRect: Qt.rect(0, 0.5, 0.5, 0.5);
                }

                ViewportEx {
                    projectionType: Viewport.FRONTAL;
                    boundingRect: Qt.rect(0, 0, 0.5, 0.5);
                }

                ViewportEx {
                    projectionType: Viewport.LEFT;
                    boundingRect: Qt.rect(0.5, 0, 0.5, 0.5);
                }
            }

            MouseArea {
                id: mouseAreaModelItem;
                property real prevMouseX: -1.0;
                property real prevMouseY: -1.0;

                enabled: true;

                anchors.fill: parent;
                onClicked: switch (mouse.button) {
                           case Qt.LeftButton: if (!!modelViewerEx.selectedPointName) {
                               modelViewer.selectedPointPosition = Qt.point(
                                           mouseX * parent.fboSize.width / width,
                                           (height - mouseY) * parent.fboSize.height / height);
                               }

                               if (mouse.modifiers && Qt.ShiftModifier) {
                                   Helpers.mouseInViewPort(mouseX, height - mouseY, Settings.ViewPorts[modelViewer]);
                               }

                               break;
                           }
                z: - 1;
            }
        }
    }

    function modelRead(buffers) {
        modelViewer.modelRead(buffers);
    }

    function drawSlices(slices) {
        modelViewer.drawSlices(slices);
    }

}
