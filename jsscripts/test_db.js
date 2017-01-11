print("\n\nbegin test_db.js");
print("------------------");

var db = new Db("testing", {onDisk:true});

print("expecting Handle for testing database: ", db);

var store = db.createDocStore("docStore", {onDisk:true});

print("expecting Handle for docStore in testing database: ", store);

var recId = store.insert({a:1, b:2});

print("expecting recordId: ", recId);
