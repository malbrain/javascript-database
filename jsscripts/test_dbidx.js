print("\n\nbegin test_dbidx.js");
print("------------------");
print("The database creator: ", Db);

var ver, cnt;
var dbops = {onDisk:true};
var db = new Db("testing", dbops);

print("Handle for: ", db);

var store = db.createDocStore("docStore", {onDisk:true});

print("Handle for: ", store);

var PrimaryIdx = store.createIndex("PrimaryIdx", {onDisk:true}, {b:"fwd:int"});
var SecondIdx = store.createIndex("SecondIdx", {onDisk:true}, {a:"fwd:dbl"});
var ThirdIdx = store.createIndex("ThirdIdx", {onDisk:true}, {x:"fwd:string"});

print("Handle: ", PrimaryIdx);
print("Handle: ", SecondIdx);
print("Handle: ", ThirdIdx);

print("\nstoring documents: ",{a:1.0, b:2, c: {d:"A", e:"F"}, x:"alpha0"});

var recId = store.insert({a:1.0, b:2, c: {d:"A", e:"F"}, x:"alpha3"});
print("recordId for insert of a:1.0, b:2, x:alpha3: ", recId);

recId = store.insert({a:1.2, b:3, c: {d:"A", e:"F"}, x:"alpha9"});
print("recordId for insert of a:1.2, b:3, x:alpha9: ", recId);

recId = store.insert({a:1.1, b:1, c: {d:"A", e:"F"}, x:"alpha0"});
print("recordId for insert of a:1.1, b:1, x:alpha0: ", recId);


var cursor1 = PrimaryIdx.createCursor();
var doc;

print("\ndocuments forward sorted by field b");
cursor1.move(CursorOp.opLeft);

while (doc = cursor1.move(CursorOp.opNext))
	print (doc.docId, "\t", doc);

print("\ndocuments reverse sorted by field b");
cursor1.move(CursorOp.opRight);

while (doc = cursor1.move(CursorOp.opPrev))
	print (doc.docId, "\t", doc);

var cursor2 = SecondIdx.createCursor();

print("\ndocuments forward sorted by field a");
cursor2.move(CursorOp.opLeft);

while (doc = cursor2.move(CursorOp.opNext))
	print (doc.docId, "\t", doc);

print("\ndocuments reverse sorted by field a");
cursor2.move(CursorOp.opRight);

while (doc = cursor2.move(CursorOp.opPrev))
	print (doc.docId, "\t", doc);

var cursor3 = ThirdIdx.createCursor();

print("\ndocuments forward sorted by field x");
cursor3.move(CursorOp.opLeft);

while (doc = cursor3.move(CursorOp.opNext))
	print (doc.docId, "\t", doc);

print("\ndocuments reverse sorted by field x");
cursor3.move(CursorOp.opRight);

while (doc = cursor3.move(CursorOp.opPrev))
	print (doc.docId, "\t", doc);
