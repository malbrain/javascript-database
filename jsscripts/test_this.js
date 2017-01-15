
function Db(dbname, options) {
    if (!this)
        return new Db(dbname, options);

    var opt = DbOptParse(Db, options);
    var handle = jsdb_openDatabase(dbname, opt);

    this.name = dbname;
    this.options = options;
    this.setValue(handle);
}

print("start");
var db = new Db("testdb");
print("db = ", db);
