print("\n\nbegin test_db.js");
print("------------------");
print("The database creator: ", Db);

var dbops = {onDisk:true};
var db = new Db("testing", dbops);

print("Handle for: ", db);

var store = db.createDocStore("docStore", {onDisk:true});

print("Handle for: ", store);

var recId = store.insert({a:1, b:2});

print("recordId for insert: ", recId);

var iterator = store.createIterator();

print("Handle for: ", iterator);

var ver, cnt;

print ("\nIterate forwards");

for (cnt = 0; ver = iterator.next(); cnt++)
	print("DocId: ", ver.docId, " -> ", ver);

print (cnt, " found forwards");

print ("\nIterate backwards");

for (cnt = 0; ver = iterator.prev(); cnt++)
	print("DocId: ", ver.docId, " -> ", ver);

print (cnt, " found backwards");

