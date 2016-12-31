//  Options array slots

var DbOptions = enum {
	OnDisk = 0,
	InitSize,
	UseTxn,
	NoDocs,
	DropDb,

	IdxKeySpec = 10,
    IdxKeySpecLen,      // this must immediately follow
    IdxKeyUnique,
    IdxKeySparse,
    IdxKeyPartial,
    IdxKeyPartialLen,   // this must immediately follow

    Btree1Bits = 20,    // Btree1 set
	Btree1Xtra,
	MaxParam = 30
};

function DbOptParse(options) {
	var optVals = new Array(DbOptions.MaxParam);

	for (var name in options)
		optVals[DbOptions[name]] = options[name];

	return optVals;
}

var optobj = {OnDisk: true, InitSize: 32768};
print ("options = ", optobj);

var opts = DbOptParse(optobj);
print ("OnDisk (expecting true) = ", opts[DbOptions.OnDisk]);
print ("InitSize (expecting 32768) = ", opts[DbOptions.InitSize]);
print ("DropDb (expecting undefined) = ", opts[DbOptions.DropDb]);
