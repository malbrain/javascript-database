win_bison.exe --output="js.tab.c" --defines="js.tab.h" --debug --verbose --warnings=all --report=state --report-file="js.tab.output"  "js.y"
win_flex.exe --wincompat -B -R -f --outfile="js.lex.c" js.l
cl /W3 /Oi /Ox /Z7 /Fejsdb.exe js*.c database/db_*.c database/btree2/btree2*.c database/btree1/btree1*.c database/artree/artree*.c wsock32.lib /Fm /link ./setargv.obj 
