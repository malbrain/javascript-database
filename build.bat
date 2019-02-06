win_bison -d -v -Wall js.y
win_flex --wincompat -B -R -f js.l
cl /W3 /DEBUG /Oi /Ox /Z7 /Fejsdb.exe js*.c lex.yy.c database/db_*.c database/btree2/btree2*.c database/btree1/btree1*.c database/artree/artree*.c wsock32.lib /Fm /link ./setargv.obj 
