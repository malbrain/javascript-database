var DbCursor = function (collection, query, projection) {
	this._collection = collection;
	this._projection = projection;
	this._query = query;
};

DbCollection.prototype.find = function (query, projection) {
	var cursor = new DbCursor(this, query, projection);
	return cursor;
};

DbCursor.prototype.sort = function (index, start, limit) {
	this._index = index;
	this._start = start;
	this._limit = limit;
	return this;
};

DbCursor.prototype.hasNext = function () {
	var document;

	if (!this._collection._docStore)
		this._collection._docStore = this._collection._install();

	if (!this._cursor)
	  if (this._index)
		this._cursor = jsdb_createCursor(this._collection._docStore[this._index], true, this._start, this._limit);
	  else
		this._cursor = jsdb_createIterator(this._collection._docStore._docStore);

	if (this._nextDocId)
		return true;

	if (this._index) {
		while (this._nextDocId = jsdb_nextKey(this._cursor, this._collection._docStore._docStore, &document))
		  if (jsdb_findDocs(this._query, document))
			return this._nextDoc = document, true;
	} else {
		while (this._nextDocId = jsdb_nextDoc(this._cursor, &document))
		  if (jsdb_findDocs(this._query, document))
			return this._nextDoc = document, true;
	}

	return false;
};

DbCursor.prototype.next = function () {
	var document;

	if (!this._collection._docStore)
		this._collection._docStore = this._collection._install();

	if (!this._cursor)
	  if (this._index)
		this._cursor = jsdb_createCursor(this._collection._docStore[this._index], true, this._start, this._limit);
	  else
		this._cursor = jsdb_createIterator(this._collection._docStore._docStore);

	if (this._nextDocId)
		return this._nextDocId = 0, this._nextDoc;

	if (this._index) {
		while (this._nextDocId = jsdb_nextKey(this._cursor, this._collection._docStore._docStore, &document))
		  if (jsdb_findDocs(this._query, document))
			return this._nextDocId = 0, this._nextDoc = document;
	} else {
		while (this._nextDocId = jsdb_nextDoc(this._cursor, &document))
		  if (jsdb_findDocs(this._query, document))
			return this._nextDocId = 0, this._nextDoc = document;
	}

	return null;
};
