var debug = true;

var Db = function(dbname) {
	this._dbname = dbname;
};

Db.catalog = {};
Db.inMem = [];

Db._defaultSize = 1024 * 1024;
Db._autoIndexId = true;
Db._onDisk = true;

var db = new Db("test");
