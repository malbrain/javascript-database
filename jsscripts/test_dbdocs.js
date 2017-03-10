print("\n\nbegin test_dbdocs.js");
print("------------------");
var ver, cnt;
var dbops = {onDisk:true};
var db = new Db("testing", dbops);

var store = db.createDocStore("docStore", {onDisk:true});

var recId = store.insert({a:1.0, b:3, c: {d:"A", e:"F"}, x:"alpha3"});
print("recordId for insert of a:1.0, b:2, c: {d:\"A\", e:\"F\"}, x:alpha3: ", recId);

var doc = store.fetch(recId);
print("document fetched for docId: ", recId, " :: ", doc);

var chg = doc.c;
print("sub-object chg = doc.c: ", chg);

chg.attr = 14;
print("c.attr = 14: ", chg);

doc.c = chg;
print("doc.c = chg: ", doc);

print("update doc: ", doc.update());

print("document fetched for docId: ", recId, " :: ", store.fetch(recId));
