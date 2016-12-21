var store, txn;
initEngine();

createRecStore(1024 * 1024, &store);
createTxn(&txn);

var count = 1;

while ( count ) {
    var record = {
        a : count,
        b : "a string body"
    };
    var id, cnt;
    var array = [ record, record ];
    insertRec(store, txn, array, &id, &cnt);
    print(id);
    count -= 1;
}

commitTxn(txn);

