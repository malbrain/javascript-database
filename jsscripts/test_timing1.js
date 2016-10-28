var store, txn, db, new;
initDatabase(&db, "test", 0, true);
openDocStore(&store, db, "basic1", 1024 * 1024, true, &new);
if(new) print("created new DocStore");


openIndex(store, &index, "_id_");
createCursor(store, snap, &iterator);

while (nextRec(iterator, &id, &record)) {
    if (record.a == 500000)
        print(id, record);
}

closeIterator(iterator);

