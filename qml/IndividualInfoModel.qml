import QtQuick 2.3

import "../js/pointsdictionary.js" as PointsDict
import "../js/helpers.js" as Helpers
import "../js/settings.js" as Settings

ListModel {
    id: individualListModel;

    property int distUpdatedCount: 0;

    Component.onCompleted: {
        function populateListElements(ptps, measures) {
            var qmlElements = [];
            var modelPoints = Settings.Points[modelID];

            if (modelID > 0) {
                Settings.Distances[modelID] = Settings.Distances[modelID] || { };
                var modelDists = Settings.Distances[modelID];
            }

            for (var pointFrom in ptps) {
                var pointsTo = ptps[pointFrom];

                for (var i = 0; i !== pointsTo.length; ++ i) {
                    var qmlElement = {};

                    var pointTo = pointsTo[i];

                    qmlElement["from"] = measures[pointFrom].name;
                    qmlElement["to"] = measures[pointTo].name;

                    // if selected point has from and to points, which are measured - assign them to appropriate ps
                    var positionFrom = undefined;
                    var positionTo = undefined;

                    if (modelPoints && modelPoints[pointFrom] && modelPoints[pointTo]) {
                        positionFrom = modelPoints[pointFrom].position;
                        positionTo = modelPoints[pointTo].position;
                    }
                    // if we know position of from and to points, calc and show it

                    if (positionFrom && positionTo) {
                        var calculatedDist = positionTo.minus(positionFrom).length();
                        qmlElement["measure"] = (+calculatedDist.toFixed(8)).toString();

                        // tell other parts about calculted dist
                        modelDists[pointFrom] = modelDists[pointFrom] || { };
                        modelDists[pointFrom][pointTo] = calculatedDist;

                        console.log(pointFrom, pointTo)

                        distUpdatedCount ++;
                    }
                    else {
                        qmlElement["measure"] = "Нет данных";
                    }

                    qmlElements.push(qmlElement);
                }
            }

            return qmlElements;
        }

        var measures = PointsDict.pointsDict.measures;
        var groups = PointsDict.pointsDict.groups;
        var groupOrder = PointsDict.groupsOrder.IndividualInfo;

        distUpdatedCount = 0;

        for (var i = 0; i !== groupOrder.length; ++ i) {
            var group = groups[groupOrder[i]];
            append({
                       "itemId" : groupOrder[i],
                       "itemName" : group.name,
                       "itemText" : group.distances.header,
                       "subItems" : populateListElements(group.distances.point2point, measures),
                       "grouped" : groupOrder[i] !== "ungrouped",
                       "elementHeight" : 0
                   });
        }
    }
}
