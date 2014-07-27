function opendb() {
    return LocalStorage.openDatabaseSync("settingsDB", "0.1", "same as the name", 10000);
}

function init() {
    var db = opendb();
    db.transaction(function(tx) {
        tx.executeSql("CREATE TABLE IF NOT EXISTS settings (setting TEXT UNIQUE, value TEXT)");
    });
}

function set(setting, value) {
    var db = opendb();
    var result = false;
    db.transaction(function(tx) {
        var rs = tx.executeSql("INSERT OR REPLACE INTO settings VALUES(?, ?);", [setting, value]);
        result = rs.rowsAffected ? true : false;
    });
    return result;
}

function get(setting) {
    var db = opendb();
    var result = null;
    db.transaction(function(tx) {
        var rs = tx.executeSql("SELECT value FROM settings WHERE setting=?;", [setting]);
        if (rs.rows.length) {
            result = rs.rows.item(0).value;
        }
    });
    return result;
}
