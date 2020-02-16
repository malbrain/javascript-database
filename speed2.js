var count = 0;
var idx;

var txn;
var start = new Date();

var db, dbname;

for (dbname in catalog.db)
	db = new Db(dbname), db.drop();

db = new Db("tstdb", {onDisk:true});

var store = db.createDocStore("collection", {onDisk:true});
var index = store.createIndex("speedIdx", {onDisk:true, idxType:0}, {doc:"fwd:dbl"});

while(count<1000) {
    var id, cnt;
    idx = 0;
	var docIds = [];

//    txn = jsdb_beginTxn();
    var array = [], key = [];

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

    docIds = store.append(array);

	for( idx = 0; idx<1000;idx++)
		index.buildKey(docIds[idx], array[idx].doc);

 //   jsdb_commitTxn();
    count += 1;
//	print ("batch: ", count);
}

var stop = new Date();
var ins = (stop - start) / 1000.;
start = stop;

var cursor, doc;

cursor = index.createCursor();

var reccnt = 0;
var prev = 0;

while( doc = cursor.move(CursorOp.opNext)) {
	if (!(reccnt % 998))
		print("idx: ", reccnt, " docId: ", doc.docId, " key: ", doc.doc);
	if (doc.doc < prev)
		print ("out of order record #", reccnt, " key: ", doc.doc, " prev: ", prev);

	prev = doc.doc;
    reccnt += 1;
}

var stop = new Date();

print ("insert: ", ins, " seconds");
print ("found: ", reccnt, " should be 1000000");
print ("sort verify: ", (stop - start) / 1000., " seconds");
