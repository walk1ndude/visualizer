import QtQuick 2.3;

import RenderTools 1.0;

import "qrc:/js/settings.js" as Settings
import "qrc:/js/helpers.js" as Helpers

ModelViewer {
    id: modelViewer;

    fboSize: Qt.size(768, 768);

    //xRange: Qt.vector2d(0.0, 1.0);
    //yRange: Qt.vector2d(0.0, 1.0);
    //zRange: Qt.vector2d(0.0, 1.0);

    rotation: Qt.vector3d(0.0, 0.0, 0.0);

    selectedPoint: ({});

    onPointUpdated: {
        // update info about points
        Settings.Points[point.modelID] = Settings.Points[point.modelID] || { };
        Settings.Points[point.modelID][point.name] = Settings.Points[point.modelID][point.name] || { };
        Settings.Points[point.modelID][point.name]["position"] = point.position;
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

        blueprint: [ {
                "type" : "EvaluatorModel",
                "params" : {
                    "width" : 10,
                    "height" : 10,
                    "stepX" : 10.0,
                    "color" : "blue"
                }
            }
        ]

        onScalingFactorChanged: modelViewerEx.scalingFactor = scalingFactor;
    }

    function processModel(model) {
        drawModel(model.data);
    }
}
