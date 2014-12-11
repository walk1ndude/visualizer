import QtQuick 2.3;

import ParserTools 1.0;

DicomReader {
    id: dicomReader;

    property var viewer: ({});

    blueprint: {
            "textures" : [ {
                    "id" : "volume"
                }
            ],
            "materials" : [ {
                    "id": "first",
                    "emissive": Qt.vector4d(1.0, 1.8, 1.0, 1.0),
                    "diffuse": Qt.vector4d(1.0, 1.0, 1.0, 1.0),
                    "specular": Qt.vector4d(0.1, 1.2, 0.2, 0.2),
                    "shininess": 100.0
                    }
                 ],
            "lightSources" : [ {
                    "id": "first",
                    "position": Qt.vector4d(0.0, 0.0, 10.0, 1.0),
                    "color": Qt.vector4d(1.0, 1.0, 1.0, 0.9),
                    "ambientIntensity": 0.05,
                    "attenuation": 0.006
                    }
                ],
            "models": [ {
                "type" : "VolumeModel",
                "params" : {
                    "children" : [
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
                        "first" : {
                            "position" : "lightSource.position",
                            "color" : "lightSource.color",
                            "ambientIntensity" : "lightSource.ambientIntensity",
                            "attenuation" : "lightSource.attenuation"
                        }
                    },
                    "materials" : {
                        "first" : {
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
                    },
                    "textures" : {
                        "volume" : {
                            "sampler" : "volume"
                        }
                    }
                }
            }
        ]
    };

    onSend: {
        viewer.message = model;
        toggleDocks();
        destroy();
    }
}
