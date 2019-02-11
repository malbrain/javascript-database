Javascript-database
===================

A working project for High-performance javascript interpreter with built-in NO-SQL document store source code in C.  The database operations are performed directly from a javascript dialect program, or submitted over tcp connections from mongo shells, or node.js connections.

```
git clone --recursive git://github.com/malbrain/javascript-database
```
Download latest commits (after cloning):

```
git pull
git submodule update --remote --recursive
```

Compilation is achieved on 64 bit linux:

```
bison -d -v -Wall js.y
flex -f js.l
gcc -std=gnu99 -Wall -Wshadow -Wpointer-arith -Wstrict-prototypes -O2 -ggdb -o jsdb -fno-omit-frame- pointer js*.c lex.yy.c database/db*.c database/btree1/*.c database/artree/*.c database/btree2/*.c -lm -lpthread -Wl ,-Map=jsdb.map
```
For 64 bit Windows: (be sure to use win_bison version 3.0.4) The win_bison and win_flex executables are included, along with a compressed file containing the data subdirectory for extraction.

```
win_bison.exe --output="js.tab.c" --defines="js.tab.h" --debug --verbose --warnings=all --report=state --report-file="js.tab.output"  "js.y"
win_flex.exe --wincompat -B -R -f --outfile="js.lex.c" js.l
cl /W3 /Oi /Ox /Z7 /Fejsdb.exe js*.c database/db_*.c database/btree2/btree2*.c database/btree1/btree1*.c database/artree/artree*.c wsock32.lib /Fm /link ./setargv.obj
```
The software also runs on the Windows Subsystem Linux (WSL) and compiles under WSL using the build.wsl bash script.

Supplied are many javascript programs to run.  The first ones are speed1.js and speed2.js which each write 1000000 documents into a collection.  speed1.js writes only the document, while speed2.js adds a random index key value to each document:

```
D:\github\javascript-database>del dbdata
D:\github\javascript-database\dbdata\*, Are you sure (Y/N)? y

D:\github\javascript-database>jsdb system*.js speed1.js
insert: 2.166 seconds
found: 1000000 should be 1000000
scan verify: 0.736 seconds

D:\github\javascript-database>del dbdata
D:\github\javascript-database\dbdata\*, Are you sure (Y/N)? y

D:\github\javascript-database>jsdb system*.js speed2.js
insert: 3.469 seconds
found: 1000000 should be 1000000
sort verify: 1.501 seconds
```
The tcp server for mongo shell clients is launched on port 27017 by running:

```
jsdb system*.js mongo*.js
```
Otherwise, a javaScript file will run with arguments:

```
jsdb -opt1 -opt2 system*.js yourfile.js -- yourarg1 yourarg2 yourarg3 ...
```
The mongod.js server program is under developement.  You can experiment with collection indexing, saving, and sorting from the mongo shell.  The collection.createIndex sample call: t.createIndex({field:1}, {type:"art"}); the save command: t.save({field:1}); and a sample find().sort() call: t.find().sort({index: "field_1", start:[composit flds ,...], limit:[composit flds...]});

A mongo direct interface that implements mongo shell commands directly on the server without BSON or a TCP connection:

```
jsdb system*.js jsDb*.js test_mongo.js
```
Please address any concerns, bug reports, or questions to the author: malbrain@cal.berkeley.edu.

