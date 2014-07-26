.pragma library

var pointsDict = {
    "measures" : {
        "incisor" : {
            "visuals" : {
                "text" : "Верхняя резцовая",
                "color" : "red"
            }
        },
        "leftjoint" : {
            "visuals" : {
                "text" : "Левый сустав",
                "color" : "yellow"
            }
        },
        "rightjoint" : {
            "visuals" : {
                "text" : "Правый сустав",
                "color" : "green"
            }
        },
        "leftprosthetic" : {
            "visuals" : {
                "text" : "Левая опорная точка протетической плоскости",
                "color" : "cyan"
            }
        },
        "rightprosthetic" : {
            "visuals" : {
                "text" : "Правая опорная точка протетической плоскости",
                "color" : "orange"
            }
        }
    },

    "groups" : {
        "bonvile" : {
            "groupHeader" : "Треугольник Бонвиля",
            "distances" : {
                "header" : "Межсуставные расстояния",
                "point2point" : {
                    "incisor" : ["leftjoint", "rightjoint"],
                    "leftjoint" : ["rightjoint"]
                }
            }
        },
        "ungroupped" : {
            "groupHeader" : "измерения",
            "distances" : {
                "header" : "ddfdf",
                "point2point" : {
                    "incisor" : ["leftprosthetic", "rightprosthetic"]
                }
            }
        }
    }
};

var measuresOrder = {
    "MeasuresGrid" : [
        "incisor",
        "leftjoint",
        "rightjoint",
        "leftprosthetic",
        "rightprosthetic"
    ]
}

var groupsOrder = {
    "IndividualInfo" : [
        "bonvile",
        "ungroupped"
    ]
}
