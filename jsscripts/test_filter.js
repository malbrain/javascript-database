var t = db.createCollection("basic1", {onDisk:false});
t.createIndex({field:1}, {name:"field", type:"art"});

for (var idx = 0; idx < 1000000; idx++)
	t.save ({field: Math.random(), _id: idx});

var cursor = t.find({_id: { $gt: 500000, $lt: 510000 }}).sort("field", [.9], [.91]);

var count = 0;

while (cursor.hasNext())
	print(cursor.next()), count++;

print ("found: ", count);
