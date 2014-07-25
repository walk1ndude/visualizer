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
        "bonville" : {
            "text" : "Треугольник Бонвиля",
            "distances" : {
                "text" : "Межсуставные расстояния",
                "point2point" : {
                    "incisor" : ["leftjoint", "rightjoint"],
                    "leftjoint" : ["rightjoint"]
                }
            }
        },
        "test" : {
            "text" : "тест",
            "distances" : {
                "text" : "тестовые расстояния",
                "point2point" : {}
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
        "bonville",
        "test"
    ]
}
