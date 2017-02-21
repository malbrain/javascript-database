print("\n\nbegin test_db.js");
print("------------------");
print("The database creator: ", Db);

var ver, cnt;
var dbops = {onDisk:true};
var db = new Db("testing", dbops);

print("Handle for: ", db);

var store = db.createDocStore("docStore", {onDisk:true});

print("Handle for: ", store);

var recId = store.insert({a:1, b:2, c: {d:"A", e:"F"}});

print("recordId for insert: ", recId);

var iterator = store.createIterator();

print("Handle for: ", iterator);

if (ver = iterator.seek(recId)) {
	print("\nver = iterator.seek(recId)");
	print("\nPrint(ver) Expecting {a:1, b:2, c: {d:\"A\", e:\"F\"}} : ", ver);
	print("\nPrint(ver.c) Expecting {d:\"A\", e:\"F\"} : ", ver.c);
}

print ("\nIterate forwards");

iterator.seek(IteratorOp.opBegin);

for (cnt = 0; ver = iterator.next(); cnt++)
	print("DocId: ", ver.docId, " -> ", ver);

print (cnt, " found forwards");

print ("\nIterate backwards");

for (cnt = 0; ver = iterator.prev(); cnt++)
	print("DocId: ", ver.docId, " -> ", ver);

print (cnt, " found backwards");
