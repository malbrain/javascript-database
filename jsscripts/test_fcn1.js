var debug = 1;
newConnection(1);

function newConnection (connId) {
    print ("new connection accepted");
    var len, id, resp, op;
    var next, nonstop = 1;
	var catalog = {};
    var respid = 0;

    do {
        respid += 1;
		op = 2004;

        if(op == 2010)
            newCommand(len, respid, id, connId, catalog);

        if(op == 2004)
            newQuery(len, respid, id, connId, catalog);

        if(op == 2002)
            newInsert(len, respid, id, connId, catalog);

    } while (nonstop);
}

function newCommand (len, respid, id, connId, catalog) {
}

function newInsert (len, respid, id, connId, catalog) {
}

function newQuery (len, respid, id, connId, catalog) {
}

