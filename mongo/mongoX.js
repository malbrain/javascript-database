var debug = 0;

print("mongoX is listening on tcp port 27017");
jsdb_tcpListen(27017, newConnection);

function newConnection (filein, fileout, connId) {
	print ("new connection accepted");
	var len, id, resp, op, collAuto;
	var next, nonstop = 1;
	var respid = 0;

	do {
		if (jsdb_readInt32(filein, &len, &id, &resp, &op))
			len -= 4 * 4;
		else
			break;

		if (debug) {
			print ("Len: ", len);
			print ("ReplyTo: ", id);
			print ("ResponseTo: ", resp);
			print ("Opcode: ", op);
		}

		respid += 1;

		if(op == 2010)
			newCommand();

		if(op == 2004)
			newQuery();

		if(op == 2002)
			newInsert();

	} while (nonstop);

  //  the sub-functions

  function newCommand () {
	var docId, docStore, dbname, commandname;
	var db, result, database;
	var flags, size, document, names;
	var array;

	var env = {};

	if (jsdb_readString(filein, &dbname)) {
		if(debug) print ("Command dbname: ", dbname);
	} else
		print ("Error on dbname");

	len -= dbname.length + 1;

	if (jsdb_readString(filein, &commandname)) {
		if(debug) print ("Command commandname: ", commandname);
	} else
		print ("Error on commandname");

	if (debug) print("newCommand command: ", commandname);
	len -= commandname.length + 1;

	if(debug)
		print("read bson command bytes left = ", len);

	if (jsdb_readBSON(filein, &document, len, &size)) {
		if(debug) print ("command document len = ", size);
	} else
		print ("Error on command document");

	if (debug)
		print (document);

	len -= size;

	env.fileout = fileout;
	env.replycode = 2011;
	env.dbname = dbname;
	env.respid = resp;
	env.id = id;

	Db[commandname](env, document[0]);
  }

  function newInsert () {
	var docId, docStore, fullname;
	var flags, documents, names;
	var count, size, array, result;

	if (jsdb_readInt32(filein, &flags)) {
		if(debug) print ("Flags: ", flags);
	} else
		return;

	len -= 4;

	if (jsdb_readString(filein, &fullname)) {
		if(debug) print ("Insert fullname: ", fullname);
	} else
		print ("Error on fullname");

	len -= fullname.length + 1;

	if(debug)
		print("read bson insert bytes left = ", len);

	if (jsdb_readBSON(filein, &documents, len, &size)) {
		if(debug) print ("Insert document len = ", size);
	} else
		print ("Error on Insert document");

	if (debug)
		print (documents);

	names = fullname.split(".");
	if(debug) print ("fullname: ", names);

	var env = { };

	env.dbname = names[0];
	env.fileout = fileout;
	env.respid = respid;
	env.replycode = 1;
	env.id = id;

	documents.insert = names[1];
	Db.insert(env, documents);
  }

  function newQuery () {
	var count, docId, docStore, fullname;
	var flags, nskip, nreturn, names;
	var size, query, selector;
	var result, array, docs;

	if (jsdb_readInt32(filein, &flags)) {
		if(debug) print ("Flags: ", flags);
	} else
		return;

	len -= 4;
	if (debug) print ("Len: ", len);

	if (jsdb_readString(filein, &fullname)) {
		if(debug) print ("Query fullname: ", fullname, " size: ", fullname.length);
	} else
		print ("Error on fullname");

	len -= fullname.length + 1;
	if (debug) print ("Len: ", len);

	if (jsdb_readInt32(filein, &nskip)) {
		if(debug) print ("Query num skip: ", nskip);
	} else
		print ("Error on nskip");

	if (jsdb_readInt32(filein, &nreturn)) {
		if (debug) print ("Query num return: ", nreturn);
	} else
		print ("Error on nreturn");

	len -= 2 * 4;

	if(debug)
		print("read bson query bytes left = ", len);

	if (jsdb_readBSON(filein, &docs, len, &size)) {
		if(debug) print ("Query document len = ", size);
	} else
		print ("Error on query document");

	query = docs[0];

	if (docs.length > 1)
		selector = docs[1];

	if (debug)
		print (query);

	len -= size;

	if (debug)
		print ("finish query command, bytes remaining = ", len);

	names = fullname.split(".");
	if(debug) print ("fullname: ", names);

	if (names[0] == "admin")
	 if( names[1] == "$cmd")
		return adminCommand(query);

	//  if this is a $cmd operation

	if( names[1] == "$cmd")
		return dbCommand(names[0], query);

	//  otherwise it's a find operation

	if(debug) print("find: ", names[0], ".", names[1], " query: ", query);
	var env = { };

	env.dbname = names[0];
	env.fileout = fileout;
	env.respid = respid;
	env.replycode = 1;
	env.id = id;

	query.find = names[1];
	Db.find(env, [query]);
  }

  function adminCommand(query) {
	if (debug) print("adminCommand: ", query);
	var env = { };

	env.fileout = fileout;
	env.respid = respid;
	env.replycode = 1;
	env.id = id;

	if (query.isMaster)
		return Db.isMaster(env, query);

	if (query.buildInfo)
		return Db.buildInfo(env, query);

	if (query.replSetGetStatus)
		return Db.replSetGetStatus(env, query);

	if (query.getLog)
		return Db.getLog(env, query);

	if (query.whatsmyuri)
		return Db.whatsmyuri(env, query);
  }

  function dbCommand(dbname, query) {
	var result, array, docStore;
	var docId, count;

	var env = { };

	env.fileout = fileout;
	env.respid = respid;
	env.dbname = dbname;
	env.replycode = 1;
	env.id = id;

	if (query.isMaster)
		return Db.isMaster(env, query);

	if (query.count)
		return Db.count(env, query);

	if (query.buildInfo)
		return Db.buildInfo(env, query);

	if (query.getlasterror) {
		result = {
				 n : 1,
				 connectionId : connId,
				 wtime : 0,
				 ok : true
			 	};

		if(debug) print(result);

		if(debug) print("response ID:", env.respid, "  responding to:", env.id);
		jsdb_response(env.fileout, env.respid, id, 8, 0, 1, 0, [result]);
		return;
	}

	if (query.insert)
		return Db.insert(env, query.documents);
  }
}
