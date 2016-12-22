print("\n\nbegin test_cursor.js");
print("------------------");
var catalog = {}, db, auto, docStore;

jsdb_initDatabase(&db, "test", 4096, false);

var database = { db : db };
catalog["test"] = database;

jsdb_createDocStore (&docStore, db, "basic1", 1024 * 1024, true, &auto);

print("catalog = ", catalog);
print("database = ", database);
print("docStore = ", docStore);

var docId, cursor, document;

jsdb_createCursor(docStore[2], &cursor, true);

while(jsdb_nextKey(cursor, &docId)) {
	jsdb_findDoc(docStore[0], docId, &document);
	print (docId, " --> ", document);
}

