import QtQuick 2.3;

import ParserTools 1.0;

StlReader {
    id: stlReader;

    property variant viewer: ({});

    blueprint: {
        "StlModel" : {
            "children" : {
                "PointsModel" : {}
            },
            "lights" : {
                "-1" : [
                        "lightSource.position",
                        "lightSource.color",
                        "lightSource.ambientIntensity",
                        "lightSource.attenuation"
                    ]
            },
            "materials" : {
                "-1" : [
                    "material.emissive",
                    "material.diffuse",
                    "material.specular",
                    "material.shininess"
                ]
            },
            "viewRangeShader" : [
                "ranges.xRange",
                "ranges.yRange",
                "range.zRange"
            ]
        }
    };

    onSend: {
        if (viewer) {
            viewer.recieve(model);
            toggleDocks();
            destroy();
        }
    }
}
