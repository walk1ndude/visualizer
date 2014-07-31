function opendb() {
    return LocalStorage.openDatabaseSync("settingsDB", "0.1", "same as the name", 10000);
}

function init() {
    var db = opendb();
    db.transaction(function(tx) {
        tx.executeSql("CREATE TABLE IF NOT EXISTS points (id INTEGER PRIMARY KEY AUTOINCREMENT, point TEXT UNIQUE, name TEXT UNIQUE, color TEXT)");
        tx.executeSql("CREATE TABLE IF NOT EXISTS groups (id INTEGER PRIMARY KEY AUTOINCREMENT, 'group' TEXT UNIQUE, name TEXT UNIQUE)");
        tx.executeSql("CREATE TABLE IF NOT EXISTS points_in_group (groupId INTEGER, pointFromId INTEGER, pointToId INTEGER, \
PRIMARY KEY(groupId, pointFromId, pointToId),
FOREIGN KEY(groupId) REFERENCES groups(id), FOREIGN KEY(pointFromId) REFERENCES points(id), FOREIGN KEY(pointToId) REFERENCES points(id))");
        tx.executeSql("CREATE TABLE IF NOT EXISTS models (id INTEGER PRIMARY KEY AUTOINCREMENT, TEXT storeLocation, rangeX TEXT, rangeY TEXT, rangeZ TEXT, \
huRange TEXT, rotation TEXT, zoomFactor REAL)");
        tx.executeSql("CREATE TABLE IF NOT EXISTS points_in_model (modelId INTEGER, pointId INTEGER, position TEXT, \
PRIMARY KEY(modelId, pointId),
FOREIGN KEY(modelId) REFERENCES models(id), FOREIGN KEY(pointId) REFERENCES points(id))");
    });
}

function serializeVector(vector) {
    switch (vector.length) {
    case 4 : return "[" + vector3d.x.toString() + "," + vector3d.y.toString() + "," + vector3d.z.toString() + "," + vector3d.w.toString() + "]";
    case 3 : return "[" + vector3d.x.toString() + "," + vector3d.y.toString() + "," + vector3d.z.toString() + "]";
    case 2 : return "[" + vector3d.x.toString() + "," + vector3d.y.toString() + "]";
    default :
        var str = "[";
        for (var i = 0; i !== vector.lenght; ++ i) {
            str += (vector[i] + ",");
        }
        return str.slice(0, -1) + "]";
    }
}

function parseVector(stringVd) {
    var vector = /d+{.d+}/.exec(stringVd);
    switch (vector.length) {
    case 4: return Qt.vector4d(vector[0], vector[1], vector[2], vector[3]);
    case 3: return Qt.vector3d(vector[0], vector[1], vector[2]);
    case 2: return Qt.vector2d(vector[0], vector[1]);
    default: return vector;
    }
}

function saveGeometry(geometry, modelId) {
    var db = opendb();
    var result = false;
    db.transaction(function(tx) {
        geometry.rotation = serializeVector(geometry.rotation);
        result = !!tx.executeSql("UPDATE models SET rotation = ?, zoomFactor = ? WHERE id = ?;",
                                 [geometry.rotation, geometry.zoomFactor, modelId]).rowsAffected;
    });
    return result;
}

function loadGeometry(modelId) {
    var db = opendb();
    var geometry = {};
    db.transaction(function(tx) {
        var query = tx.executeSql("SELECT FROM models (rotation, rangeX, rangeY, rangeZ, huRange, zoomFactor) \
WHERE id = ?;", [modelId]);
        if (query.rows.length) {
            for (column in query.item(0)) {
                geometry.column = (column !== "zoomFactor") ? parseVector(column) : column;
            }
        }
    });
    return geometry;
}

var Points = {}