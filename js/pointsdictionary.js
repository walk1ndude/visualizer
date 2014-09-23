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
        "leftprothetic" : {
            "name" : "Левая опорная точка протетической плоскости",
            "color" : "cyan"
        },
        "rightprothetic" : {
            "name" : "Правая опорная точка протетической плоскости",
            "color" : "orange"
        },
        "belowincisor" : {
            "name" : "Нижняя резцовая",
            "color" : "green"
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
                    "incisor" : ["leftprothetic", "rightprothetic"]
                }
            }
        }
    }
};

var measuresOrder = {
    "Measures" : [
        "incisor",
        "leftjoint",
        "rightjoint",
        "leftprothetic",
        "rightprothetic",
        "belowincisor"
    ]
}

var groupsOrder = {
    "Individual" : [
        "bonvile",
        "ungrouped"
    ]
}
