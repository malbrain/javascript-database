print("\n\nbegin test_dbidx.js");
print("------------------");
print("The database creator: ", Db);

var db, dbname;

for (dbname in catalog.db)
	db = new Db(dbname), db.drop();

var ver, cnt;
var dbops = {onDisk:true};
db = new Db("testing", dbops);

print("Handle for: ", db);

var store = db.createDocStore("docStore", {onDisk:true});

print("Handle for: ", store);

var PrimaryIdx = store.createIndex("PrimaryIdx", {onDisk:true}, {b:"fwd:int"});
var SecondIdx = store.createIndex("SecondIdx", {onDisk:true}, {a:"fwd:dbl"});
var ThirdIdx = store.createIndex("ThirdIdx", {onDisk:true}, {x:"fwd:string"});
var FourthIdx = store.createIndex("FourthIdx", {onDisk:true}, {"c.d":"fwd:string"});
var FifthIdx = store.createIndex("FifthIdx", {onDisk:true}, {"yy":"fwd:string"});

print("Handle: ", PrimaryIdx);
print("Handle: ", SecondIdx);
print("Handle: ", ThirdIdx);
print("Handle: ", FourthIdx);
print("Handle: ", FifthIdx);

print("\nstoring documents: ",{a:1.0, b:2, c: {d:"B", e:"F"}, x:"alpha0"});

var doc = store.insert({a:1.0, b:2, c: {d:"A", e:"F"}, x:"alpha3"});
print("recordId for insert of a:1.0, b:2, c.d:A x:alpha3: ", doc.docId);

doc = store.insert({a:1.2, b:3, c: {d:"Z", e:"F"}, x:"alpha9"});
print("recordId for insert of a:1.2, b:3, c.d:Z x:alpha9: ", doc.docId);

doc = store.insert({a:1.1, b:1, c: {d:"M", e:"F"}, x:"alpha0"});
print("recordId for insert of a:1.1, b:1, c.d:M x:alpha0: ", doc.docId);

var cursor1 = PrimaryIdx.createCursor({deDup:true});

print("\ndocuments forward sorted by field b");
cursor1.move(CursorOp.opLeft);

while (doc = cursor1.next())
	print (doc.docId, "\t", doc);

print("\ndocuments reverse sorted by field b");
cursor1.move(CursorOp.opRight);

while (doc = cursor1.prev())
	print (doc.docId, "\t", doc);

var cursor2 = SecondIdx.createCursor();

print("\ndocuments forward sorted by field a");
cursor2.move(CursorOp.opLeft);

while (doc = cursor2.next())
	print (doc.docId, "\t", doc);

print("\ndocuments reverse sorted by field a");
cursor2.move(CursorOp.opRight);

while (doc = cursor2.prev())
	print (doc.docId, "\t", doc);

var cursor3 = ThirdIdx.createCursor();

print("\ndocuments forward sorted by field x");
cursor3.move(CursorOp.opLeft);

while (doc = cursor3.next())
	print (doc.docId, "\t", doc);

print("\ndocuments reverse sorted by field x");
cursor3.move(CursorOp.opRight);

while (doc = cursor3.prev())
	print (doc.docId, "\t", doc);

var cursor4 = FourthIdx.createCursor();
var doc;

print("\ndocuments forward sorted by field c.d");
cursor4.move(CursorOp.opLeft);

while (doc = cursor4.next())
	print (doc.docId, "\t", doc);

print("\ndocuments reverse sorted by field c.d");
cursor4.move(CursorOp.opRight);

while (doc = cursor4.prev())
	print (doc.docId, "\t", doc);

