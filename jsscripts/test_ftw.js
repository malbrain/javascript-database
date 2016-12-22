print("\n\nbegin test_ftw.js");
print("------------------");

var list = jsdb_listFiles(".");
print("Expecting sorted current directory: ", list.sort());
