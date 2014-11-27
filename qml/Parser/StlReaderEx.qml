import QtQuick 2.3;

import ParserTools 1.0;

StlReader {
    id: stlReader;

    property variant viewer: ({});

    blueprint: [ {
            "type" : "StlModel",
            "params" : {
                "children" : [
                    {
                        "type" : "PointsModel",
                        "params" : {}
                    },
                    {
                        "type" : "AxesModel",
                        "params" : {
                            "axesColor" : {
                                "x" : "red",
                                "y" : "green",
                                "z" : Qt.rgba(0, 0, 1, 1)
                            },
                            "length" : 1.5
                        }
                    }
                ],
                "lights" : {
                    "-1" : {
                        "position" : "lightSource.position",
                        "color" : "lightSource.color",
                        "ambientIntensity" : "lightSource.ambientIntensity",
                        "attenuation" : "lightSource.attenuation"
                    }
                },
                "materials" : {
                    "-1" : {
                        "emissive" : "material.emissive",
                        "diffuse" : "material.diffuse",
                        "specular" : "material.specular",
                        "shineness" : "material.shininess"
                    }
                },
                "viewRangeShader" : {
                    "x" : "ranges.xRange",
                    "y" : "ranges.yRange",
                    "z" : "ranges.zRange"
                }
            }
        }
    ];

    onSend: {
        if (viewer) {
            viewer.recieve(model);
            toggleDocks();
            destroy();
        }
    }
}
