var count = 0;
var idx;

var txn;
var start = new Date();

var db = new Db("tstdb", {onDisk:true});

var store = db.createDocStore("collection", {onDisk:true});
var index = store.createIndex("speedIdx", {onDisk:true}, {doc:"fwd:int"});

while(count<1000) {
    var id, cnt;
    idx = 0;

//    txn = jsdb_beginTxn();
    var array = [];

    while(idx<1000) {
//		print ("batch: ", count, " item: ", idx);
        array[idx] = {
           doc : Math.random() * (count * 1000 + idx),
           cnt : count,
           idx : idx,
           text0 : "This is a test string designed to make this record bigger0",
           text1 : "This is a test string designed to make this record bigger1",
           text2 : "This is a test string designed to make this record bigger2",
           text3 : "This is a test string designed to make this record bigger3",
           text4 : "This is a test string designed to make this record bigger4",
           text5 : "This is a test string designed to make this record bigger5",
           text6 : "This is a test string designed to make this record bigger6",
           text7 : "This is a test string designed to make this record bigger7",
           text8 : "This is a test string designed to make this record bigger8",
           text9 : "This is a test string designed to make this record bigger9"
           };
        idx += 1;
    }

    store.insert(array);
 //   jsdb_commitTxn();
    count += 1;
//	print ("batch: ", count);
}

var stop = new Date();
print ("insert: ", (stop - start) / 1000., " seconds");
start = stop;

var cursor, doc;

cursor = index.createCursor();

var reccnt = 0;

while( doc = cursor.move(CursorOp.opNext)) {
	if (!(reccnt % 998))
		print("idx: ", reccnt, " docId: ", doc.docId, " key: ", doc.doc, ":", doc.text1);
    reccnt += 1;
}

var stop = new Date();

print ("found: ", reccnt, " should be 1000000");
print ("scan: ", (stop - start) / 1000., " seconds");
