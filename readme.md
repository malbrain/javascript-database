Javascript-database
===================

A working project for High-performance javascript and database source code in C.  The database operations are performed directly from a javascript dialect program, or over tcp connections from mongo shells, or node.js connections.

> `git clone --recursive git://github.com/malbrain/javascript-database`

Compilation is achieved on 64 bit linux:

> `bison -d -v -Wall js.y`
> `flex -f js.l`
> `gcc -std=gnu99 -Wall -Wshadow -Wpointer-arith -Wstrict-prototypes -O2 -ggdb -o jsdb -fno-omit-frame- pointer js*.c lex.yy.c database/db*.c database/btree1/*.c database/artree/*.c -lm -lpthread -Wl ,-Map=jsdb.map`

and for 64 bit Windows:

> `win_bison -d -v -Wall js.y`
> `win_flex --wincompat -f js.l`
> `cl /Ox /Fejsdb.exe js*.c lex.yy.c database/db*.c database/artree/artree*.c database/btree1/btree1*.c wsock32.lib /link setargv.obj`

The tcp server for mongo shell clients is launched on port 27017 by running:

> `jsdb system*.js mongo*.js`

Otherwise, a javaScript file will run with arguments:

> `jsdb -opt1 -opt2 system*.js yourfile.js -- yourarg1 yourarg2 yourarg3 ...`

The mongod.js server program is under developement.  You can experiment with collection indexing, saving, and sorting from the mongo shell.  The collection.createIndex sample call: t.createIndex({field:1}, {type:"art"}); the save command: t.save({field:1}); and a sample find().sort() call: t.find().sort({index: "field_1", start:[composit flds ,...], limit:[composit flds...]});

A mongo direct interface that implements mongo shell commands directly on the server without BSON or a TCP connection:

> `jsdb system*.js jsDb*.js test_mongo.js`

Please address any concerns, bug reports, or questions to the author: malbrain@cal.berkeley.edu.

