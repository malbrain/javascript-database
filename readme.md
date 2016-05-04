Javascript-database
===================

A working project for High-performance database source code in C.  The database operations are performed directly from a javascript dialect program, or over tcp connections from mongo shells.

Compilation is achieved on 64 bit linux:

bison -d -v -Wall jsdb.y

flex --wincompat -f jsdb.l

gcc -O3 -o jsdb jsdb*.c -lpthread

and for 64 bit Windows:

win_bison -d -v -Wall jsdb.y

win_flex --wincompat -f jsdb.l

cl /Ox /Fejsdb.exe jsdb*.c lex.yy.c wsock32.lib /link setargv.obj

The tcp server for mongo shell clients is launched on port 27017 by running:

jsdb system*.js mongo*.js

Otherwise, a javaScript file will run with arguments:

jsdb system*.js yourfile.js -- yourarg1 yourarg2 yourarg3 ...

The mongod.js server program is under developement.  You can experiment with collection indexing, saving, and sorting from the mongo shell.  The collection.createIndex sample call: t.createIndex({field:1}, {type:"art"}); the save command: t.save({field:1}); and a sample find().sort() call: t.find().sort({index: "field_1", start:[composit flds ,...], limit:[composit flds...]});

A mongo direct interface that implements mongo shell commands directly on the server without BSON or a TCP connection:

jsdb system*.js jsDb*.js test_mongo.js

Please address any concerns, bug reports, or questions to the author: malbrain@cal.berkeley.edu.

