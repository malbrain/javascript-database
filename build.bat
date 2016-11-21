win_bison -d -v -Wall js.y
win_flex --wincompat -f js.l
cl /Od /Z7 /Fejsdb.exe js*.c lex.yy.c database/db_*.c database/btree1/btree1*.c database/artree/artree*.c wsock32.lib /Fm /link setargv.obj 
