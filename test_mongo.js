var t = db.getCollection("basic2");

t.createIndex({field:1}, {name : "field", type: "art", size: 1024 * 1024});

t.save ({field:1, _id:"a1230"});
t.save ({field:100, _id:"a1231"});
t.save ({field:3, _id:"a1232"});
t.save ({field:300, _id:"a1233"});
t.save ({field:2, _id:"a1234"});
t.save ({field:200, _id:"a1235"});

var cursor1 = t.find().sort("field");

while (cursor1.hasNext())
	print(cursor1.next());

var t = db.getCollection("basic3");

t.createIndex({field:1}, {name : "field", type: "art", size: 1024 * 1024});

for (var idx = 0; idx < 1000000; idx++)
	t.save ({field: Math.random(), _id: idx});

var cursor2 = t.find().sort("field");

while (cursor2.hasNext())
	print(cursor2.next());

