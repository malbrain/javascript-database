print("\n\nbegin test_db.js");
print("------------------");
print("The database creator: ", Db);

var db = new Db("testing", {onDisk:true});

print("Handle for testing database: ", db);

var store = db.createDocStore("docStore", {onDisk:true});

print("Handle for docStore in testing: ", store);

var recId = store.insert({a:1, b:2});

print("recordId for insert: ", recId);

var iterator = store.createIterator();

print("Handle for Iterator for testing.documents: ", iterator);

var ver;

while (ver = iterator.next())
	print(ver);


