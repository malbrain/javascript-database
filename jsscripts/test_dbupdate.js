print("\n\nbegin test_dbupdate.js");
print("------------------");

var list = jsdb_listFiles("dbdata");

for (var file of list)
	if (file.startsWith("testing"))
		jsdb_deleteFile("dbdata/" + file);

jsdb_deleteFile("dbdata/Txns");

var doc, ver, cnt;
var dbops = {onDisk:true};
var db = new Db("testing", dbops);

print("Handle for: ", db);

var store = db.createDocStore("docStore", {onDisk:true});

print("Handle for: ", store);

var PrimaryIdx = store.createIndex("PrimaryIdx", {onDisk:true}, {b:"fwd:int"});
var SecondIdx = store.createIndex("SecondIdx", {onDisk:true}, {a:"fwd:dbl"});
var ThirdIdx = store.createIndex("ThirdIdx", {onDisk:true}, {x:"fwd:string"});
var FourthIdx = store.createIndex("FourthIdx", {onDisk:true}, {"c.d":"fwd:string"});
var FifthIdx = store.createIndex("FifthIdx", {onDisk:true}, {"yy":"fwd:int"});

print("\nstoring documents: ",{a:1.0, b:2, c: {d:"B", e:"F"}, x:"alpha0"});

doc = store.insert({a:1.0, b:3, c: {d:"A", e:"F"}, x:"alpha3"});
print("recordId for insert of a:1.0, b:2, c.d:A x:alpha3: ", doc.docId);

doc = store.insert({a:1.2, b:1, c: {d:"Z", e:"F"}, x:"alpha9"});
print("recordId for insert of a:1.2, b:3, c.d:Z x:alpha9: ", doc.docId);

doc = store.insert({a:1.1, b:2, c: {d:"M", e:"F"}, x:"alpha0"});
print("recordId for insert of a:1.1, b:1, c.d:M x:alpha0: ", doc.docId);

var cursor1 = PrimaryIdx.createCursor();

print("\ndocuments forward sorted by field b");
cursor1.move(CursorOp.opLeft);

while (doc = cursor1.move(CursorOp.opNext))
	print (doc.docId, "\t", doc);

print("\ndocuments reverse sorted by field b");

cursor1.move(CursorOp.opRight);

while (doc = cursor1.move(CursorOp.opPrev))
	print (doc.docId, "\t", doc);

// re-run from left to right

print("\ndocuments updated with field yy");

var id = 1;

while (doc = cursor1.move(CursorOp.opNext)) {
	doc.yy = 2 * id++;
	print("update: ", doc.docId, "\t", doc.update());
}

var cursor2 = FifthIdx.createCursor({cursorDeDup:true});

print ("\nfwd list on field yy of updated yy integer field:");

var nxt;

while(nxt = cursor2.move(CursorOp.opNext))
	print(doc = nxt);

cursor2.reset();

print ("\nrev list on field yy of updated yy integer field:");

cursor2.move(CursorOp.opRight);

while(nxt = cursor2.move(CursorOp.opPrev))
	print(doc = nxt);

print ("\nstress test 1000000 updates of the doc.yy integer field key");
var start = Date();

id = 0;

while (id < 1000000) {
	doc.yy = 4 * id++;
	doc.update();
}

print ("elapsed time: ", (Date() - start) / 1000., " seconds");

print ("\nfwd list on field yy of updated yy integer field:");

cursor2.reset();

while(nxt = cursor2.move(CursorOp.opNext))
	print(doc = nxt);

print ("\nstress test 1000000 updates of the doc.c.e integer field non-key");
var start = Date();

id = 0;

while (id < 1000000) {
	doc.c.e = 5 * id++;
	doc.update();
}

print ("elapsed time: ", (Date() - start) / 1000., " seconds");

print ("\nfwd list on field yy integer field:");

cursor2.reset();

while(doc = cursor2.move(CursorOp.opNext))
	print(doc);

