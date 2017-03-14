print("\n\nbegin test_db.js");
print("------------------");

var list = jsdb_listFiles("dbdata");

for (var file of list)
	if (file.startsWith("testing"))
		jsdb_deleteFile("dbdata/" + file);

jsdb_deleteFile("dbdata/Txns");

print("The database creator: ", Db);

var doc, cnt;
var dbops = {onDisk:true};
var db = new Db("testing", dbops);

print("Handle for: ", db);

var store = db.createDocStore("docStore", {onDisk:true});

print("Handle for: ", store);

doc = store.insert({a:1, b:2, c: {d:"A", e:"F"}});

print("recordId for insert: ", doc.docId);

var iterator = store.createIterator();

print("Handle for: ", iterator);

print("\nPrint(doc) Expecting {a:1, b:2, c: {d:\"A\", e:\"F\"}} : ", doc);
print("\nPrint(doc.c) Expecting {d:\"A\", e:\"F\"} : ", doc.c);

print ("\nIterate forwards");

iterator.seek(IteratorOp.opBegin);

for (cnt = 0; doc = iterator.next(); cnt++)
	print("DocId: ", doc.docId, " -> ", doc);

print (cnt, " found forwards");

print ("\nIterate backwards");

for (cnt = 0; doc = iterator.prev(); cnt++)
	print("DocId: ", doc.docId, " -> ", doc);

print (cnt, " found backwards");
