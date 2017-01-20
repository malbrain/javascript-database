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

var ver;

while (ver = iterator.next())
	print("Doc: ", ver.docId, " -> ", ver);

