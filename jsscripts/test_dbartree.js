print("\n\nbegin test_dbartree.js");
print("------------------");

var db, dbname;

for (dbname in catalog.db)
	db = new Db(dbname), db.drop();

var dbops = {onDisk:true};
db = new Db("testing", dbops);

var store = db.createDocStore("artree", {onDisk:true});

var PrimaryIdx = store.createIndex("PrimaryArtree1", {idxType:HndlType.ArtIndex, onDisk:true}, {field:"fwd:dbl"});

store.insert ({field:1.0, _id:"a1230"});
store.insert ({field:100.0, _id:"a1231"});
store.insert ({field:3.0, _id:"a1232"});
store.insert ({field:300.0, _id:"a1233"});
store.insert ({field:2.0, _id:"a1234"});
store.insert ({field:200.0, _id:"a1235"});

var cursor1 = PrimaryIdx.createCursor({deDup:true});
var doc;

while (doc = cursor1.move(CursorOp.opNext))
	print(doc);

var store = db.createDocStore("artree3", {onDisk:true});

var PrimaryIdx = store.createIndex("PrimaryArtree2", {idxType:HndlType.ArtIndex, onDisk:true}, {field:"fwd:dbl"});

print("begin insert of 1M documents with random field arttree index");
var start = Date();

for (var idx = 0; idx < 1000000; idx++)
	store.insert ({field: Math.random(), _id: idx});

print("elapsed time: ", (Date() - start)/1000., " seconds");

print("\nbegin cursor check of 1M documents with random field arttree index");

var cursor2 = PrimaryIdx.createCursor();

start = Date();
var prev = 0.0;
var cnt = 0;

while (doc = cursor2.move(CursorOp.opNext)) {
	if (doc.field < prev)
		print(prev, "==>", doc.field);
	prev = doc.field;
	cnt++;
}

print("elapsed time: ", (Date() - start)/1000., " seconds");
print("end cursor check of ", cnt, " documents with random field arttree index");
