var count = 0;
var idx;

var txn;
var start = new Date();

var db, dbname;

for (dbname in catalog.db)
	db = new Db(dbname), db.drop();

db = new Db("tstdb", {onDisk:true});

var store = db.createDocStore("collection", {onDisk:true, recordType:1});
var index = store.createIndex("speedIdx", {onDisk:true, idxType:1}, {doc:"fwd:dbl"});

while(count<1000) {
    var txn, id, cnt;
    idx = 0;
	var docIds = [];

    txn = new Txn();
    var array = [], key = [], keys;

    while(idx<1000) {
//		print ("batch: ", count, " item: ", idx);
        array[idx] = {
           doc : Math.random() * (count * 1000 + idx),
           cnt : count,
           idx : idx,
/*           text0 : "This is a test string designed to make this record bigger0",
           text1 : "This is a test string designed to make this record bigger1",
           text2 : "This is a test string designed to make this record bigger2",
           text3 : "This is a test string designed to make this record bigger3",
           text4 : "This is a test string designed to make this record bigger4",
           text5 : "This is a test string designed to make this record bigger5",
           text6 : "This is a test string designed to make this record bigger6",
           text7 : "This is a test string designed to make this record bigger7",
           text8 : "This is a test string designed to make this record bigger8",
           text9 : "This is a test string designed to make this record bigger9"
*/           };
        idx += 1;
    }

    docIds = txn.write(store, array);
	var nxt;

	for( idx = 0; idx<1000;idx++) {
		keys = index.buildKey(docIds[idx], array[idx].doc);
		for( nxt = 0; nxt < keys.length; nxt++ )
			index.insertKey(docIds[idx], keys[nxt++]);
	}

	print("keys:", keys, " docId: ", docIds[idx - 1]);

    txn.commit();
    count += 1;
//	print ("batch: ", count);
}

var stop = new Date();
var ins = (stop - start) / 1000.;
start = stop;

var cursor, doc, docId;

cursor = index.createCursor();

var reccnt = 0;
var prev = 0;

while( doc = cursor.move(CursorOp.opNext)) {
	if (!(reccnt % 2500))
		print("idx: ", reccnt, " doc.docId: ", doc.docId, "\tkey: ", doc.doc);
	if (doc.doc < prev)
		print ("out of order record #", reccnt, "\tkey: ", doc.doc, " prev: ", prev);

	prev = doc.doc;
    reccnt += 1;
}

var stop = new Date();

print ("insert: ", ins, " seconds");
print ("found: ", reccnt, " should be 1000000");
print ("sort verify: ", (stop - start) / 1000., " seconds");
