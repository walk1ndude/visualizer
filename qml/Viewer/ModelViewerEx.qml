import QtQuick 2.3;

import RenderTools 1.0;

import "qrc:/js/settings.js" as Settings
import "qrc:/js/helpers.js" as Helpers

Item {
    id: modelViewerEx;

    property vector2d xRange: Qt.vector2d(0.0, 1.0);
    property vector2d yRange: Qt.vector2d(0.0, 1.0);
    property vector2d zRange: Qt.vector2d(0.0, 1.0);

    property vector3d rotation: Qt.vector3d(0.0, 0.0, 0.0);
    property real zoomFactor: 2.0;

    property int minHU: 0;
    property int maxHU: 65535;

    property variant selectedPoint: ({});

    property int modelID: modelViewer.modelID;

    property real scalingFactor: 1.0;

    signal pointUpdated(variant point);

    function togglePoint(point) {
        modelViewer.togglePoint(point);
    }

    ModelViewer {
        id: modelViewer;

        width: modelViewerEx.width;
        height: modelViewerEx.height;

        fboSize: Qt.size(768, 768);

        xRange: modelViewerEx.xRange;
        yRange: modelViewerEx.yRange;
        zRange: modelViewerEx.zRange;

        minHU: modelViewerEx.minHU;
        maxHU: modelViewerEx.maxHU;

        selectedPoint: modelViewerEx.selectedPoint;

        onPointUpdated: {
            // update info about points
            Settings.Points[point.modelID] = Settings.Points[point.modelID] || { };
            Settings.Points[point.modelID][point.name] = Settings.Points[point.modelID][point.name] || { };
            Settings.Points[point.modelID][point.name]["position"] = point.position;

            // need to update sidebar info, so give singal about it
            modelViewerEx.pointUpdated(point);
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

        modelScene: ModelScene {
            id: modelScene;

            viewportArray: viewportArray;

            LightSource {
                position: Qt.vector4d(0.0, 0.0, 10.0, 1.0);
                color: Qt.vector4d(1.0, 1.0, 1.0, 0.9);
                ambientIntensity: 0.05;
                attenuation: 0.006;
            }

            Material {
                emissive: Qt.vector4d(1.0, 1.8, 1.0, 1.0);
                diffuse: Qt.vector4d(1.0, 1.0, 1.0, 1.0);
                specular: Qt.vector4d(0.1, 1.2, 0.2, 0.2);
                shininess: 100.0;
            }

            onScalingFactorChanged: modelViewerEx.scalingFactor = scalingFactor;
        }
    }

    function modelRead(buffers) {
        modelViewer.modelRead(buffers);
    }

    function drawSlices(slices) {
        modelViewer.drawSlices(slices);
    }

}
