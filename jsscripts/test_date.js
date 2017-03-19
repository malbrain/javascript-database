print("\n\nbegin test_date.js");
print("------------------");
print("expect now                 : ", Date());

var now = new Date();
now += 2000;
print("expect now + 2 seconds     : ", now);

date.setTime(1000);
print("expect 1/1/1970 - tz + 1sec: ", date);
