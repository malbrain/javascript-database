var count = 0;
var idx;

var txn;

var db = jsdb_openDatabase("tstdb", true);
var store = jsdb_openDocStore(db, "collection", 1024 * 1024, true);

var keys = { doc:1 };
var index = jsdb_createIndex(store, keys, "_id", "artree", 0, true, false);

while(count<1000) {
    var id, cnt;
    idx = 0;

    txn = jsdb_beginTxn(db);
    var array = [];

    while(idx<10) {
        array[idx] = {
           doc : count * 1000 + idx,
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

    jsdb_insertDocs(store, array, &id, &cnt, txn);
    jsdb_commitTxn(db, txn);
    count += 1;
	print ("batch: ", count);
}

var iterator, record;

var query = { doc : {$not : {$gt:10}}, $or: [{doc : {$in:[0,1]}}, {doc : 9}]};

  iterator = jsdb_createIterator(store);
  var reccnt = 0;
  var record;

  while( jsdb_nextDoc(iterator, &record) )
    if (jsdb_findDocs(query, record)) {
        print("match doc: ", record.doc);
        reccnt += 1;
    }

//  closeIterator(iterator);
  print ("matches: ", reccnt, " should be 3");
