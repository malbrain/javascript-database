var t = db.createCollection("btree2", {onDisk:true});

t.createIndex({field:1}, {name : "field", type: "btree", size: 1024 * 1024});

t.save ({field:1.0, _id:"a1230"});
t.save ({field:100.0, _id:"a1231"});
t.save ({field:3.0, _id:"a1232"});
t.save ({field:300.0, _id:"a1233"});
t.save ({field:2.0, _id:"a1234"});
t.save ({field:200.0, _id:"a1235"});

var cursor1 = t.find().sort("field");

while (cursor1.hasNext())
	print(cursor1.next());

var t = db.createCollection("btree3", {onDisk:true});

t.createIndex({field:1}, {name : "field", type: "btree", size: 1024 * 1024});

for (var idx = 0; idx < 1000000; idx++)
	t.save ({field: Math.random(), _id: idx});

var cursor2 = t.find().sort("field");
var prev = 0.0;

while (cursor2.hasNext()) {
	var doc = cursor2.next();
	if (doc.field < prev)
		print(prev, "==>", doc.field);
	prev = doc.field;
}
