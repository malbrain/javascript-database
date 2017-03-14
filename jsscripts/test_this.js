print("\n\nbegin test_this.js");
print("------------------");

var list = jsdb_listFiles("dbdata");

for (var file of list)
	if (file.startsWith("testing"))
		jsdb_deleteFile("dbdata/" + file);

jsdb_deleteFile("dbdata/Txns");


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
var db = new Db("testing", {onDisk:false});
print("db = ", db);
