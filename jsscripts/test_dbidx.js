print("\n\nbegin test_dbidx.js");
print("------------------");
print("The database creator: ", Db);

var ver, cnt;
var dbops = {onDisk:true};
var db = new Db("testing", dbops);

print("Handle for: ", db);

var store = db.createDocStore("docStore", {onDisk:true});

print("Handle for: ", store);

var index = store.createIndex("PrimaryIdx", {onDisk:true}, {b:"fwd:string"});

print("Handle for: ", index);

print("\nstoring documents: ",{a:1, b:2, c: {d:"A", e:"F"}});

var recId = store.insert({a:1, b:2, c: {d:"A", e:"F"}});
print("recordId for insert of b:2: ", recId);

recId = store.insert({a:1, b:3, c: {d:"A", e:"F"}});
print("recordId for insert of b:3: ", recId);

recId = store.insert({a:1, b:1, c: {d:"A", e:"F"}});
print("recordId for insert of b:1: ", recId);


var cursor = index.createCursor();

print("Handle for: ", cursor);

var doc;

print("\ndocuments sorted by field b");
cursor.move(CursorOp.opLeft);

while (doc = cursor.move(CursorOp.opNext))
	print (doc);

print("\ndocuments reverse sorted by field b");
cursor.move(CursorOp.opRight);

while (doc = cursor.move(CursorOp.opPrev))
	print (doc);
