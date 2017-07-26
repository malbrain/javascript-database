print("\n\nbegin test_dbtxn.js");
print("------------------");

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
var FifthIdx = store.createIndex("FifthIdx", {onDisk:true}, {"yy":"fwd:int"});

print("Handle for: ", PrimaryIdx);
print("Handle for: ", SecondIdx);
print("Handle for: ", ThirdIdx);
print("Handle for: ", FourthIdx);
print("Handle for: ", FifthIdx);

print("\nstoring documents in txn: ",{a:1.0, b:2, c: {d:"B", e:"F"}, x:"alpha0"});
var txn = beginTxn();
print("\nbeginTxn: ", txn);

var doc = store.insert({a:1.0, b:3, c: {d:"A", e:"F"}, x:"alpha3"});
print("recordId for insert of a:1.0, b:2, c.d:A x:alpha3: ", doc.docId);

doc = store.insert({a:1.2, b:1, c: {d:"Z", e:"F"}, x:"alpha9"});
print("recordId for insert of a:1.2, b:3, c.d:Z x:alpha9: ", doc.docId);

doc = store.insert({a:1.1, b:2, c: {d:"M", e:"F"}, x:"alpha0"});
print("recordId for insert of a:1.1, b:1, c.d:M x:alpha0: ", doc.docId);

print("commitTxn: ", txn, ", Txn cnt: ", txn.count, ", Key1: ", PrimaryIdx.count, ' Key2: ', SecondIdx.count, ' Key3: ', ThirdIdx.count, ' Key4: ', FourthIdx.count, ' Key5: ', FifthIdx.count, "\n");
commitTxn();

var docId = doc.docId;

var cursor1 = PrimaryIdx.createCursor();
var doc, nxt;

print("documents forward sorted by key b: ", cursor1);
cursor1.move(CursorOp.opLeft);

while (doc = cursor1.move(CursorOp.opNext))
	print (doc.docId, "\t", doc);

print("\ndocuments reverse sorted by key b: ", cursor1);

cursor1.move(CursorOp.opRight);

while (doc = cursor1.move(CursorOp.opPrev))
	print (doc.docId, "\t", doc);

// re-run from left to right

print("\ndocuments updated with key yy integer value");
txn = beginTxn();
print("\nbeginTxn: ", txn);

var id = 1;

while (doc = cursor1.move(CursorOp.opNext)) {
	doc.yy = 2 * id++;
	print("update: ", doc.docId, "\t", doc.update());
}

print("commitTxn: ", txn, ", Txn cnt: ", txn.count, ", Key1: ", PrimaryIdx.count, ' Key2: ', SecondIdx.count, ' Key3: ', ThirdIdx.count, ' Key4: ', FourthIdx.count, ' Key5: ', FifthIdx.count, "\n");
commitTxn();

var cursor2 = FifthIdx.createCursor({cursorDeDup:true});

print ("fwd list on key yy of updated yy integer field:");

while(doc = cursor2.move(CursorOp.opNext))
	print(doc);

cursor2.reset();

print ("\nrev list on key yy of updated yy integer field:");

cursor2.move(CursorOp.opRight);

while(doc = cursor2.move(CursorOp.opPrev))
	print(doc);

print ("\nstress test 1000000 updates of the doc.yy integer key");
var start = Date();

var iterator = store.createIterator();
doc = iterator.seek(docId);

txn = beginTxn();
print("\nbeginTxn: ", txn, " updating doc: ", doc);

id = 0;

while (id < 1000000) {
	doc.yy = 4 * id++;
	doc.update();
}

print("commitTxn: ", txn, ", Txn cnt: ", txn.count, ", Key1: ", PrimaryIdx.count, ' Key2: ', SecondIdx.count, ' Key3: ', ThirdIdx.count, ' Key4: ', FourthIdx.count, ' Key5: ', FifthIdx.count, "\n");
commitTxn();

var stop = Date();
print ("elapsed time: ", (stop - start) / 1000., " seconds\n");

print ("fwd list on key yy of updated yy integer field:");

cursor2.reset();

while(doc = cursor2.move(CursorOp.opNext))
	print(doc);

cursor2.reset();

print ("\nrev list on key yy of updated yy integer field:");

cursor2.move(CursorOp.opRight);

while(doc = cursor2.move(CursorOp.opPrev))
	print(doc);

print ("\nstress test 1000000 updates of the doc.c.e integer non-key");
start = Date();
doc = iterator.seek(docId);

txn = beginTxn();
print("\nbeginTxn: ", txn, " updating doc: ", doc);

id = 0;

while (id < 1000000) {
	doc.c.e = 5 * id++;
	doc.update();
}

print("commitTxn: ", txn, ", Txn cnt: ", txn.count, ", Key1: ", PrimaryIdx.count, ' Key2: ', SecondIdx.count, ' Key3: ', ThirdIdx.count, ' Key4: ', FourthIdx.count, ' Key5: ', FifthIdx.count, "\n");
commitTxn();

stop = Date();
print ("elapsed time: ", (stop - start) / 1000., " seconds\n");

print ("fwd list on key yy updated integer field:");

cursor2.reset();

while(doc = cursor2.move(CursorOp.opNext))
	print(doc);

cursor2.reset();

print ("\nrev list on key yy updated integer field:");

cursor2.move(CursorOp.opRight);

while(doc = cursor2.move(CursorOp.opPrev))
	print(doc);

print ("\nIterate forwards");
iterator.seek(IteratorOp.opBegin);

for (cnt = 0; doc = iterator.next(); cnt++)
	print("DocId: ", doc.docId, " -> ", doc);

print ("\nIterate backwards");
iterator.seek(IteratorOp.opEnd);

for (cnt = 0; doc = iterator.prev(); cnt++)
	print("DocId: ", doc.docId, " -> ", doc);



