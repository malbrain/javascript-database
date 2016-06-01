var DbCursor = function (collection, query, projection, reverse) {
	this._collection = collection;
	this._projection = projection;
	this._reverse = reverse;
	this._query = query;
};

DbCollection.prototype.find = function (query, projection) {
	var cursor = new DbCursor(this, query, projection, false);
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

	if (!this._collection._docStore._hndl)
		this._collection._docStore = this._collection._install();

	if (!this._cursor)
	  if (this._index)
		this._cursor = jsdb_createCursor(this._collection._docStore[this._index], this._reverse, this._start, this._limit);
	  else
		this._cursor = jsdb_createIterator(this._collection._docStore._hndl);

	if (this._doc)
		return true;

	if (this._index) {
		while (this._nextDoc = jsdb_nextKey(this._cursor))
		  if (jsdb_findDocs(this._query, this._nextDoc))
			return true;
	} else {
		while (this._nextDoc = jsdb_nextDoc(this._cursor))
		  if (jsdb_findDocs(this._query, this._nextDoc))
			return true;
	}

	return false;
};

DbCursor.prototype.next = function () {
	var document;

	if (!this._collection._docStore)
		this._collection._docStore = this._collection._install();

	if (!this._cursor)
	  if (this._index)
		this._cursor = jsdb_createCursor(this._collection._docStore[this._index], this._reverse, this._start, this._limit);
	  else
		this._cursor = jsdb_createIterator(this._collection._docStore._hndl);

	if (document = this._nextDoc)
		return this._nextDoc = null, document;

	if (this._index) {
		while (this._nextDoc = jsdb_nextKey(this._cursor))
		  if (jsdb_findDocs(this._query, this._nextDoc))
			return this._nextDoc;
	} else {
		while (this._nextDoc = jsdb_nextDoc(this._cursor))
		  if (jsdb_findDocs(this._query, document))
			return this._nextDoc;
	}

	return null;
};
