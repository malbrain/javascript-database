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

print("\nstoring documents in txn: ",{a:1.0, b:2, c: {d:"B", e:"F"}, x:"alpha0"});
var txn = beginTxn();
print("\nbeginTxn: ", txn);

var doc = store.insert({a:1.0, b:3, c: {d:"A", e:"F"}, x:"alpha3"});
print("recordId for insert of a:1.0, b:2, c.d:A x:alpha3: ", doc);

doc = store.insert({a:1.2, b:1, c: {d:"Z", e:"F"}, x:"alpha9"});
print("recordId for insert of a:1.2, b:3, c.d:Z x:alpha9: ", doc);

doc = store.insert({a:1.1, b:2, c: {d:"M", e:"F"}, x:"alpha0"});
print("recordId for insert of a:1.1, b:1, c.d:M x:alpha0: ", doc);

print("commitTxn: ", txn, ", Txn cnt: ", txn.count, ", Key cnt: ", FifthIdx.count, "\n");
commitTxn();

var cursor1 = PrimaryIdx.createCursor();
var doc, nxt;

print("documents forward sorted by field b");
cursor1.move(CursorOp.opLeft);

while (doc = cursor1.move(CursorOp.opNext))
	print (doc.docId, "\t", doc);

print("\ndocuments reverse sorted by field b");

cursor1.move(CursorOp.opRight);

while (doc = cursor1.move(CursorOp.opPrev))
	print (doc.docId, "\t", doc);

// re-run from left to right

print("\ndocuments updated with field yy integer value");
txn = beginTxn();
print("\nbeginTxn: ", txn);

var id = 1;

while (doc = cursor1.move(CursorOp.opNext)) {
	doc.yy = 2 * id++;
	print("update: ", doc.docId, "\t", doc.update());
}

print("commitTxn: ", txn, ", Txn cnt: ", txn.count, ", Key cnt: ", FifthIdx.count, "\n");
commitTxn();

var cursor2 = FifthIdx.createCursor({cursorDeDup:true});

print ("fwd list on field yy of updated yy integer field:");

while(doc = cursor2.move(CursorOp.opNext))
	print(doc);

cursor2.reset();

print ("\nrev list on field yy of updated yy integer field:");

cursor2.move(CursorOp.opRight);

while(nxt = cursor2.move(CursorOp.opPrev))
	print(doc = nxt);

print ("\nstress test 1000000 updates of the doc.yy integer field key");
var start = new Date();

txn = beginTxn();
print("\nbeginTxn: ", txn);

id = 0;

while (id < 1000000) {
	doc.yy = 4 * id++;
	doc.update();
}

print("commitTxn: ", txn, ", Txn cnt: ", txn.count, ", Key cnt: ", FifthIdx.count, "\n");
commitTxn();

var stop = new Date();
print ("elapsed time: ", (stop - start) / 1000., " seconds\n");

print ("fwd list on field yy of updated yy integer field:");

cursor2.reset();

while(nxt = cursor2.move(CursorOp.opNext))
	print(doc = nxt);

print ("\nstress test 1000000 updates of the doc.c.e integer field non-key");
start = new Date();

txn = beginTxn();
print("\nbeginTxn: ", txn);

id = 0;

while (id < 1000000) {
	doc.c.e = 5 * id++;
	doc.update();
}

print("commitTxn: ", txn, ", Txn cnt: ", txn.count, ", Key cnt: ", FifthIdx.count, "\n");
commitTxn();

stop = new Date();
print ("elapsed time: ", (stop - start) / 1000., " seconds\n");

print ("fwd list on field yy integer field:");

cursor2.reset();

while(doc = cursor2.move(CursorOp.opNext))
	print(doc);

