Javascript-database
===================

A working project for High-performance database source code in C.  The database operations are performed directly from a javascript dialect program, or over tcp connections from mongo shells.

Compilation is achieved on linux or Windows by:

bison -d -v -Wall jsdb.y
flex --wincompat -f jsdb.l
gcc -O3 -o jsdb jsdb*.c -lpthread

or

win_bison -d -v -Wall jsdb.y
win_flex --wincompat -f jsdb.l
cl /Ox jsdb*.c

The tcp server for mongo shell clients is launched on port 27017 by running:

jsdb mongod.js

Please address any concerns, bug reports, or questions to the author: malbrain@cal.berkeley.edu.

