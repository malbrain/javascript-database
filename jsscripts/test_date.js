print("\n\nbegin test_date.js");
print("------------------");
var date = new Date();
print("expect now                 : ", date);

var now = new Date();
now += 2000;
print("expect now + 2 seconds     : ", now);

date.setTime(1000);
print("expect 1/1/1970 - tz + 1sec: ", date);
