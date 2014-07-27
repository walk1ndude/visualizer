.pragma library

var pointsDict = {
    "measures" : {
        "incisor" : {
            "name" : "Верхняя резцовая",
            "color" : "red"
        },
        "leftjoint" : {
            "name" : "Левый сустав",
            "color" : "yellow"
        },
        "rightjoint" : {
            "name" : "Правый сустав",
            "color" : "green"
        },
        "leftprosthetic" : {
            "name" : "Левая опорная точка протетической плоскости",
            "color" : "cyan"
        },
        "rightprosthetic" : {
            "name" : "Правая опорная точка протетической плоскости",
            "color" : "orange"
        }
    },

    "groups" : {
        "bonvile" : {
            "name" : "Треугольник Бонвиля",
            "distances" : {
                "header" : "Межсуставные расстояния",
                "point2point" : {
                    "incisor" : ["leftjoint", "rightjoint"],
                    "leftjoint" : ["rightjoint"]
                }
            }
        },
        "ungrouped" : {
            "distances" : {
                "header" : "Измеренные расстояния",
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
        "ungrouped"
    ]
}
