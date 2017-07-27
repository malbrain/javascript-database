print("\n\nbegin test_dbbtree.js");
print("------------------");

var db, dbname;

for (dbname in catalog.db)
	db = new Db(dbname), db.drop();

var dbops = {onDisk:true};
db = new Db("testing", dbops);

var store = db.createDocStore("btree2", {onDisk:true});

var PrimaryIdx = store.createIndex("PrimaryBtree1", {idxType:HndlType.Btree1Index, onDisk:true}, {field:"fwd:dbl"});

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

var store = db.createDocStore("btree3", {onDisk:true});

var PrimaryIdx = store.createIndex("PrimaryBtree2", {idxType:HndlType.Btree1Index, onDisk:true}, {field:"fwd:dbl"});

var start = Date();

print("begin insert of 1M documents with random field btree index");

for (var idx = 0; idx < 1000000; idx++)
	store.insert ({field: Math.random(), _id: idx});

print("elapsed time: ", (Date() - start)/1000., " seconds");

print("\nbegin cursor check of 1M documents with random field btree index");

var cursor2 = PrimaryIdx.createCursor();

var prev = 0.0;
var cnt = 0;
start = Date();

while (doc = cursor2.move(CursorOp.opNext)) {
	if (doc.field < prev)
		print(prev, "==>", doc.field);
	prev = doc.field;
	cnt++;
}

print("elapsed time: ", (Date() - start)/1000., " seconds");
print("end cursor check of ", cnt, " documents with random field btree index");

